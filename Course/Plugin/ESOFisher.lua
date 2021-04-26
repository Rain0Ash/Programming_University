
ESOFisher = {
    Name = "ESOFisher",
    CurrentState = 0,
    Angle = 0,
    Swimming = false,
    SavedVariables = nil,
    State = {},
    Defaults = {}
}



ESOFisher.State = {
    Waiting = 0,
    Lookaway = 1,
    Looking = 2,
    Depleted = 3,
    Nobait = 5,
    Fishing = 6,
    Reelin = 7,
    Loot = 8,
    Invfull = 9,
    Fight = 14,
    Dead = 15
}



ESOFisher.Defaults = {
    Enabled = true,
    PosX = 0,
    PosY = 0,
    Anchor = 3,
    AnchorRelative = 3,
    Colors = {
        [0] = { icon = "waiting", text = "Waiting", r = 1, g = 1, b = 1, a = 1 },
        [1] = { icon = "waiting", text = "Looking Away", r = 0.3, g = 0, b = 0.3, a = 1 },
        [2] = { icon = "looking", text = "Looking at Fishing Hole", r = 0.3961, g = 0.2706, b = 0, a = 1 },
        [3] = { icon = "depleted", text = "Fishing Hole Depleted", r = 0, g = 0.3, b = 0.3, a = 1 },
        [5] = { icon = "nobait", text = "Bait Not Equipped", r = 1, g = 0.8, b = 0, a = 1 },
        [6] = { icon = "fishing", text = "Fishing", r = 0.2980, g = 0.6118, b = 0.8392, a = 1 },
        [7] = { icon = "reelin", text = "Reel In!", r = 0, g = 0.8, b = 0, a = 1 },
        [8] = { icon = "loot", text = "Loot Menu", r = 0, g = 0, b = 0.8, a = 1 },
        [9] = { icon = "invfull", text = "Inventory Full", r = 0, g = 0, b = 0.2, a = 1 },
        [14] = { icon = "fight", text = "Fighting", r = 0.8, g = 0, b = 0, a = 1 },
        [15] = { icon = "dead", text = "Dead", r = 0, g = 0, b = 0, a = 1 }
    }
}



local LAM2 = LibAddonMenu2

local function ChangeState(state, overwrite)

    if ESOFisher.CurrentState == state then
        return
    end

    if ESOFisher.CurrentState == ESOFisher.State.Fight and not overwrite then
        return
    end

    if ESOFisher.Swimming and state == ESOFisher.State.Looking then
        state = ESOFisher.State.Lookaway
    end

    EVENT_MANAGER:UnregisterForUpdate(ESOFisher.Name .. "STATE_REELIN")
    EVENT_MANAGER:UnregisterForUpdate(ESOFisher.Name .. "STATE_FISHING")
    EVENT_MANAGER:UnregisterForUpdate(ESOFisher.Name .. "STATE_DEPLETED")
    EVENT_MANAGER:UnregisterForEvent(ESOFisher.Name .. "OnSlotUpdate", EVENT_INVENTORY_SINGLE_SLOT_UPDATE)

    if state == ESOFisher.State.Depleted then
        EVENT_MANAGER:RegisterForUpdate(ESOFisher.Name .. "STATE_DEPLETED", 3000, function()
            if ESOFisher.CurrentState == ESOFisher.State.Depleted then
                ChangeState(ESOFisher.State.Waiting)
            end
        end)

    elseif state == ESOFisher.State.Fishing then
        ESOFisher.Angle = (math.deg(GetPlayerCameraHeading()) - 180) % 360

        if not GetSetting_Bool(SETTING_TYPE_LOOT, LOOT_SETTING_AUTO_LOOT) then
            LOOT_SCENE:RegisterCallback("StateChange", OnLoot)
        end
        
        EVENT_MANAGER:RegisterForEvent(ESOFisher.Name .. "OnSlotUpdate", EVENT_INVENTORY_SINGLE_SLOT_UPDATE, function()
            if ESOFisher.CurrentState == ESOFisher.State.Fishing then
                ChangeState(ESOFisher.State.Reelin)
            end
        end)
        
        EVENT_MANAGER:RegisterForUpdate(ESOFisher.Name .. "STATE_FISHING", 28000, function()
            if ESOFisher.CurrentState == ESOFisher.State.Fishing then
                ChangeState(ESOFisher.State.Waiting)
            end
        end)

    elseif state == ESOFisher.State.Reelin then
        EVENT_MANAGER:RegisterForUpdate(ESOFisher.Name .. "STATE_REELIN", 3000, function()
            if ESOFisher.CurrentState == ESOFisher.State.Reelin then
                ChangeState(ESOFisher.State.Waiting)
            end
        end)
    end

    ESOFisher.UI.Icon:SetTexture("ESOFisher/textures/" .. ESOFisher.SavedVariables.Colors[state].icon .. ".dds")
    ESOFisher.UI.blocInfo:SetColor(ESOFisher.SavedVariables.Colors[state].r, ESOFisher.SavedVariables.Colors[state].g, ESOFisher.SavedVariables.Colors[state].b, ESOFisher.SavedVariables.Colors[state].a)
    ESOFisher.CurrentState = state
    ESOFisher.CallbackManager:FireCallbacks(ESOFisher.Name .. "StateChange", ESOFisher.CurrentState)
end



local function LootRelease()
    local action, _, _, _, additionalInfo = GetGameCameraInteractableActionInfo()
    local angle = ((math.deg(GetPlayerCameraHeading()) - 180) % 360) - ESOFisher.Angle

    if action and additionalInfo == ADDITIONAL_INTERACT_INFO_FISHING_NODE then
        ChangeState(ESOFisher.State.Looking)
    
    elseif action then
        ChangeState(ESOFisher.State.Lookaway)
    
    elseif -30 < angle and angle < 30 then
        ChangeState(ESOFisher.State.Depleted)
    
    else
        ChangeState(ESOFisher.State.Waiting)

    end
end



function OnLoot(_, state)
    
    if state == "showing" then
        if (GetBagUseableSize(BAG_BACKPACK) - GetNumBagUsedSlots(BAG_BACKPACK)) <= 0 then
            ChangeState(ESOFisher.State.Invfull)
        else
            ChangeState(ESOFisher.State.Loot)
        end
        
    elseif state == "hiding" then
        LootRelease()
        LOOT_SCENE:UnregisterCallback("StateChange", OnLoot)
        
    end
end



local current_name = ""
local current_moving = true

function OnAction()
    local action, name, _, _, additionalInfo = GetGameCameraInteractableActionInfo()

    if action and IsPlayerTryingToMove() and ESOFisher.CurrentState < ESOFisher.State.Fishing then
        ChangeState(ESOFisher.State.Lookaway)
        current_name = ""
        current_moving = true
        
        EVENT_MANAGER:RegisterForUpdate(ESOFisher.Name .. "MOVING", 400, function()
            EVENT_MANAGER:UnregisterForUpdate(ESOFisher.Name .. "MOVING")
            if not IsPlayerTryingToMove() then
                current_moving = false
            end
        end)

    elseif action and additionalInfo == ADDITIONAL_INTERACT_INFO_FISHING_NODE then
        if not GetFishingLure() then
            ChangeState(ESOFisher.State.Nobait)
            
        elseif ESOFisher.CurrentState < ESOFisher.State.Fishing and not current_moving then
            ChangeState(ESOFisher.State.Looking)
            current_name = name
            
        end

    elseif action and current_name == name then
        if ESOFisher.CurrentState > ESOFisher.State.Fishing then
            return
        end
        
        ChangeState(ESOFisher.State.Fishing)

    elseif action then
        ChangeState(ESOFisher.State.Lookaway)
        current_name = ""

    elseif ESOFisher.CurrentState == ESOFisher.State.Reelin and GetSetting_Bool(SETTING_TYPE_LOOT, LOOT_SETTING_AUTO_LOOT) then
        LootRelease()

    elseif ESOFisher.CurrentState ~= ESOFisher.State.Depleted then
        ChangeState(ESOFisher.State.Waiting)
        current_name = ""
    
    end
end



local function InitializeUserInterface()
    ESOFisher.UI = WINDOW_MANAGER:CreateControl(nil, GuiRoot, CT_TOPLEVELCONTROL)
    ESOFisher.UI:SetMouseEnabled(true)
    ESOFisher.UI:SetClampedToScreen(true)
    ESOFisher.UI:SetMovable(true)
    ESOFisher.UI:SetDimensions(64, 92)
    ESOFisher.UI:SetDrawLevel(0)
    ESOFisher.UI:SetDrawLayer(DL_MAX_VALUE - 1)
    ESOFisher.UI:SetDrawTier(DT_MAX_VALUE - 1)
    ESOFisher.UI:SetHidden(not ESOFisher.SavedVariables.Enabled)

    ESOFisher.UI:ClearAnchors()
    ESOFisher.UI:SetAnchor(ESOFisher.SavedVariables.AnchorRelative, GuiRoot, ESOFisher.SavedVariables.Anchor, ESOFisher.SavedVariables.PosX, ESOFisher.SavedVariables.PosY)

    ESOFisher.UI.blocInfo = WINDOW_MANAGER:CreateControl(nil, ESOFisher.UI, CT_TEXTURE)
    ESOFisher.UI.blocInfo:SetDimensions(64, 6)
    ESOFisher.UI.blocInfo:SetColor(ESOFisher.SavedVariables.Colors[ESOFisher.CurrentState].r, ESOFisher.SavedVariables.Colors[ESOFisher.CurrentState].g, ESOFisher.SavedVariables.Colors[ESOFisher.CurrentState].b, ESOFisher.SavedVariables.Colors[ESOFisher.CurrentState].a)
    ESOFisher.UI.blocInfo:SetAnchor(TOP, ESOFisher.UI, TOP, 0, blocInfo)
    ESOFisher.UI.blocInfo:SetHidden(not ESOFisher.SavedVariables.Enabled)
    ESOFisher.UI.blocInfo:SetDrawLevel(0)
    ESOFisher.UI.blocInfo:SetDrawLayer(DL_MAX_VALUE)
    ESOFisher.UI.blocInfo:SetDrawTier(DT_MAX_VALUE)

    ESOFisher.UI.Icon = WINDOW_MANAGER:CreateControl(nil, ESOFisher.UI, CT_TEXTURE)
    ESOFisher.UI.Icon:SetBlendMode(TEX_BLEND_MODE_ALPHA)
    ESOFisher.UI.Icon:SetTexture("ESOFisher/textures/" .. ESOFisher.SavedVariables.Colors[ESOFisher.CurrentState].icon .. ".dds")
    ESOFisher.UI.Icon:SetDimensions(64, 64)
    ESOFisher.UI.Icon:SetAnchor(TOPLEFT, ESOFisher.UI, TOPLEFT, 0, 18)
    ESOFisher.UI.Icon:SetHidden(not ESOFisher.SavedVariables.Enabled)
    ESOFisher.UI.Icon:SetDrawLevel(0)
    ESOFisher.UI.blocInfo:SetDrawLayer(DL_MAX_VALUE)
    ESOFisher.UI.blocInfo:SetDrawTier(DT_MAX_VALUE)

    ESOFisher.UI:SetHandler("OnMoveStop", function()
        _, ESOFisher.SavedVariables.AnchorRelative, _, ESOFisher.SavedVariables.Anchor, ESOFisher.SavedVariables.PosX, ESOFisher.SavedVariables.PosY, _ = ESOFisher.UI:GetAnchor()
    end, ESOFisher.Name)

    local fragment = ZO_SimpleSceneFragment:New(ESOFisher.UI)
    HUD_SCENE:AddFragment(fragment)
    HUD_UI_SCENE:AddFragment(fragment)
    LOOT_SCENE:AddFragment(fragment)
end



local function CreateMenu()
    local panel_name = "ESOFisherSettingsPanel"

    local panel_data = {
        type = "panel",
        name = ESOFisher.Name,
        displayName = ESOFisher.Name,
        author = "Rain0Ash",
        registerForRefresh = true,
        registerForDefaults = true,
    }
    
    local _ = LAM2:RegisterAddonPanel(panel_name, panel_data)
    local options = {
        {
            type = "description",
            text = "Here you can setup ESOFisher configs."
        },
        {
            type = "checkbox",
            name = "Enabled",
            default = true,
            disabled = false,
            
            getFunc = function()
                return ESOFisher.SavedVariables.Enabled
            end,
            
            setFunc = function(value)
                ESOFisher.SavedVariables.Enabled = value
                ESOFisher.UI:SetHidden(not ESOFisher.SavedVariables.Enabled)
                ESOFisher.UI.blocInfo:SetHidden(not ESOFisher.SavedVariables.Enabled)
                ESOFisher.UI.Icon:SetHidden(not ESOFisher.SavedVariables.Enabled)
            end
        },
        {
            type = "header",
            name = "Colors"
        },
    }

    for k, _ in pairs(ESOFisher.Defaults.Colors) do
        local default = ESOFisher.Defaults.Colors[k]
        local color = ESOFisher.SavedVariables.Colors[k]
        options[#options + 1] = {
            type = "colorpicker",
            name = default.text,
            getFunc = function()
                return color.r, color.g, color.b, color.a
            end,
            
            setFunc = function(r, g, b, a)
                color.r, color.g, color.b, color.a = r, g, b, a
                ESOFisher.SavedVariables.Colors[k] = color
                ESOFisher.UI.blocInfo:SetColor(color.r, color.g, color.b, color.a)
            end,
            
            default = { ["r"] = default.r, ["g"] = default.g, ["b"] = default.b, ["a"] = default.a },
        }
    end

    LAM2:RegisterOptionControls(panel_name, options)
end



local function AddonLoad(_, name)
    if (ESOFisher.Name ~= name) then
        return
    end
    
    EVENT_MANAGER:UnregisterForEvent(ESOFisher.Name, EVENT_ADD_ON_LOADED)

    if libMainMenuSubcategoryButton then
        libMainMenuSubcategoryButton:SetDrawLayer(DL_MIN_VALUE)
        libMainMenuSubcategoryButton:SetDrawTier(DT_MIN_VALUE)
    end

    ESOFisher.SavedVariables = ZO_SavedVars:NewAccountWide("ESOFisherSV", 1, nil, ESOFisher.Defaults)
    ESOFisher.CallbackManager = ZO_CallbackObject:New()
    
    ESOFisher.CurrentState = ESOFisher.State.Waiting

    InitializeUserInterface()
    CreateMenu()

    ZO_PreHookHandler(RETICLE.interact, "OnEffectivelyShown", OnAction)
    ZO_PreHookHandler(RETICLE.interact, "OnHide", OnAction)

    EVENT_MANAGER:RegisterForEvent(ESOFisher.Name, EVENT_PLAYER_SWIMMING, function(_)
        ESOFisher.Swimming = true
    end)
    
    EVENT_MANAGER:RegisterForEvent(ESOFisher.Name, EVENT_PLAYER_NOT_SWIMMING, function(_)
        ESOFisher.Swimming = false
    end)
    
    EVENT_MANAGER:RegisterForEvent(ESOFisher.Name, EVENT_PLAYER_DEAD, function(_)
        ChangeState(ESOFisher.State.Dead, true)
    end)
    
    EVENT_MANAGER:RegisterForEvent(ESOFisher.Name, EVENT_PLAYER_ALIVE, function(_)
        ChangeState(ESOFisher.State.Waiting)
    end)

    EVENT_MANAGER:RegisterForEvent(ESOFisher.Name, EVENT_PLAYER_COMBAT_STATE, function(_, combat)
        if combat then
            ChangeState(ESOFisher.State.Fight)

        elseif ESOFisher.CurrentState == ESOFisher.State.Fight then
            ChangeState(ESOFisher.State.Waiting, true)

        end
    end)
end

EVENT_MANAGER:RegisterForEvent(ESOFisher.Name, EVENT_ADD_ON_LOADED, function(...)
    AddonLoad(...)
end)
