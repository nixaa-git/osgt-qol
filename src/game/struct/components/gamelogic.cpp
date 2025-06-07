#include "gamelogic.hpp"
#include "game/struct/app.hpp"
#include "game/signatures.hpp"

bool GameLogicComponent::IsDialogOpened()
{
    Entity* pEntRoot = real::GetApp()->m_entityRoot;
    if (!pEntRoot)
        return false;

    Entity* pGUIEnt = pEntRoot->GetEntityByName("GUI");
    if (!pGUIEnt)
        return false;

    if (pGUIEnt->GetEntityByName("GenericDialog"))
        return true;

    Entity* pWorldGUI = pGUIEnt->GetEntityByName("WorldSpecificGUI");
    if (pWorldGUI)
    {
        if (pWorldGUI->GetEntityByName("StoreContainer"))
            return true;
        if (pWorldGUI->GetEntityByName("HelpMenuContainer"))
            return true;
        if (pWorldGUI->GetEntityByName("SurveyContainer"))
            return true;
        if (pWorldGUI->GetEntityByName("CommunityHubContainer"))
            return true;
    }
    return false;
}
