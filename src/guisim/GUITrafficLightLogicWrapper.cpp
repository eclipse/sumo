/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUITrafficLightLogicWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Oct/Nov 2003
/// @version $Id$
///
// A wrapper for tl-logics to allow their visualisation and interaction
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSLane.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/FuncBinding_StringParam.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUITLLogicPhasesTrackerWindow.h>
#include <gui/GUIGlobals.h>
#include "GUITrafficLightLogicWrapper.h"
#include "GUINet.h"
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu)
GUITrafficLightLogicWrapperPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPHASES,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_TRACKPHASES,            GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_SWITCH_OFF,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLS2Off),
    FXMAPFUNCS(SEL_COMMAND, MID_SWITCH, MID_SWITCH + 20, GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic),
};

// Object implementation
FXIMPLEMENT(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu, GUIGLObjectPopupMenu, GUITrafficLightLogicWrapperPopupMenuMap, ARRAYNUMBER(GUITrafficLightLogicWrapperPopupMenuMap))


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::GUITrafficLightLogicWrapperPopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent,
    GUIGlObject& o)
    : GUIGLObjectPopupMenu(app, parent, o) {}


GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::~GUITrafficLightLogicWrapperPopupMenu() {}



long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases(
    FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->begin2TrackPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases(
    FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->showPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLS2Off(
    FXObject*, FXSelector /*sel*/, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->switchTLSLogic(-1);
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic(
    FXObject*, FXSelector sel, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->switchTLSLogic(FXSELID(sel) - MID_SWITCH);
    return 1;
}



/* -------------------------------------------------------------------------
 * GUITrafficLightLogicWrapper - methods
 * ----------------------------------------------------------------------- */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapper(
    MSTLLogicControl& control, MSTrafficLightLogic& tll) :
    GUIGlObject(GLO_TLLOGIC, tll.getID()),
    myTLLogicControl(control), myTLLogic(tll) {}


GUITrafficLightLogicWrapper::~GUITrafficLightLogicWrapper() {}


GUIGLObjectPopupMenu*
GUITrafficLightLogicWrapper::getPopUpMenu(GUIMainWindow& app,
        GUISUMOAbstractView& parent) {
    myApp = &app;
    GUIGLObjectPopupMenu* ret = new GUITrafficLightLogicWrapperPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    const MSTLLogicControl::TLSLogicVariants& vars = myTLLogicControl.get(myTLLogic.getID());
    std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
    if (logics.size() > 1) {
        std::vector<MSTrafficLightLogic*>::const_iterator i;
        int index = 0;
        for (i = logics.begin(); i != logics.end(); ++i, ++index) {
            if (!vars.isActive(*i)) {
                new FXMenuCommand(ret, ("Switch to '" + (*i)->getProgramID() + "'").c_str(),
                                  GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, (FXSelector)(MID_SWITCH + index));
            }
        }
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, "Switch off", GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_SWITCH_OFF);
    new FXMenuCommand(ret, "Track Phases", nullptr, ret, MID_TRACKPHASES);
    new FXMenuCommand(ret, "Show Phases", nullptr, ret, MID_SHOWPHASES);
    new FXMenuSeparator(ret);
    MSTrafficLightLogic* tll = myTLLogicControl.getActive(myTLLogic.getID());
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    new FXMenuCommand(ret, ("phase: " + toString(tll->getCurrentPhaseIndex())).c_str(), nullptr, nullptr, 0);
    new FXMenuSeparator(ret);
    buildShowParamsPopupEntry(ret, false);
    buildPositionCopyEntry(ret, false);
    return ret;
}


void
GUITrafficLightLogicWrapper::begin2TrackPhases() {
    GUITLLogicPhasesTrackerWindow* window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
                                          new FuncBinding_StringParam<MSTLLogicControl, std::pair<SUMOTime, MSPhaseDefinition> >
                                          (&MSNet::getInstance()->getTLSControl(), &MSTLLogicControl::getPhaseDef, myTLLogic.getID()));
    window->create();
    window->show();
}


void
GUITrafficLightLogicWrapper::showPhases() {
    GUITLLogicPhasesTrackerWindow* window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
                                          static_cast<MSSimpleTrafficLightLogic&>(myTLLogic).getPhases());
    window->setBeginTime(0);
    window->create();
    window->show();
}


GUIParameterTableWindow*
GUITrafficLightLogicWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 3 + (int)myTLLogic.getParametersMap().size());
    ret->mkItem("tlLogic [id]", false, myTLLogic.getID());
    ret->mkItem("program", false, myTLLogic.getProgramID());
    // close building
    ret->closeBuilding(&myTLLogic);
    return ret;
}


Boundary
GUITrafficLightLogicWrapper::getCenteringBoundary() const {
    Boundary ret;
    const MSTrafficLightLogic::LaneVectorVector& lanes = myTLLogic.getLaneVectors();
    for (const auto & lanes2 : lanes) {
        for (auto j : lanes2) {
            ret.add(j->getShape()[-1]);
        }
    }
    ret.grow(20);
    return ret;
}


void
GUITrafficLightLogicWrapper::switchTLSLogic(int to) {
    if (to == -1) {
        myTLLogicControl.switchTo(myTLLogic.getID(), "off");
        MSTrafficLightLogic* tll = myTLLogicControl.getActive(myTLLogic.getID());
        GUINet::getGUIInstance()->createTLWrapper(tll);
    } else {
        const MSTLLogicControl::TLSLogicVariants& vars = myTLLogicControl.get(myTLLogic.getID());
        std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
        myTLLogicControl.switchTo(myTLLogic.getID(), logics[to]->getProgramID());
    }
}


int
GUITrafficLightLogicWrapper::getLinkIndex(const MSLink* const link) const {
    return myTLLogic.getLinkIndex(link);
}


void
GUITrafficLightLogicWrapper::drawGL(const GUIVisualizationSettings& s) const {
    if (s.gaming) {
        if (!MSNet::getInstance()->getTLSControl().isActive(&myTLLogic)) {
            return;
        };
        const std::string& curState = myTLLogic.getCurrentPhaseDef().getState();
        if (curState.find_first_of("gG") == std::string::npos) {
            // no link is 'green' at the moment. find those that turn green next
            const MSTrafficLightLogic::Phases& phases = myTLLogic.getPhases();
            int curPhaseIdx = myTLLogic.getCurrentPhaseIndex();
            int phaseIdx = (curPhaseIdx + 1) % phases.size();
            std::vector<int> nextGreen;
            while (phaseIdx != curPhaseIdx) {
                const std::string& state = phases[phaseIdx]->getState();
                for (int linkIdx = 0; linkIdx < (int)state.size(); linkIdx++) {
                    if ((LinkState)state[linkIdx] == LINKSTATE_TL_GREEN_MINOR ||
                            (LinkState)state[linkIdx] == LINKSTATE_TL_GREEN_MAJOR) {
                        nextGreen.push_back(linkIdx);
                    }
                }
                if (nextGreen.size() > 0) {
                    break;
                }
                phaseIdx = (phaseIdx + 1) % phases.size();
            }
            // highlight nextGreen links
            for (std::_Vector_iterator<std::_Vector_val<std::_Simple_types<int> > >::value_type & it_idx : nextGreen) {
                const MSTrafficLightLogic::LaneVector& lanes = myTLLogic.getLanesAt(it_idx);
                for (auto lane : lanes) {
                    glPushMatrix();
                    // split circle in red and yellow
                    Position pos = lane->getShape().back();
                    glTranslated(pos.x(), pos.y(), GLO_MAX);
                    double rot = RAD2DEG(lane->getShape().angleAt2D((int)lane->getShape().size() - 2)) - 90;
                    glRotated(rot, 0, 0, 1);
                    GLHelper::setColor(s.getLinkColor(LINKSTATE_TL_RED));
                    GLHelper::drawFilledCircle(lane->getWidth() / 2., 8, -90, 90);
                    GLHelper::setColor(s.getLinkColor(LINKSTATE_TL_YELLOW_MAJOR));
                    GLHelper::drawFilledCircle(lane->getWidth() / 2., 8, 90, 270);
                    glPopMatrix();
                }
            }
        }
    }
}


/****************************************************************************/

