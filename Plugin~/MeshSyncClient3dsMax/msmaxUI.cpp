#include "pch.h"
#include "msmaxContext.h"
#include "msmaxUtils.h"
#include "resource.h"

#define msmaxTitle L"UnityMeshSync"

#define msmaxMenuTitle_Window           L"Window"
#define msmaxMenuTitle_Cache            L"Export Cache"
#define msmaxMenuTitle_ExportScene      L"Send Scene"
#define msmaxMenuTitle_ExportAnimations L"Send Animations"
#define msmaxMenuTitle_Import           L"Import"
#define msmaxActionID_Window            1
#define msmaxActionID_Cache             2
#define msmaxActionID_ExportScene       3
#define msmaxActionID_ExportAnimations  4
#define msmaxActionID_Import            5

static const ActionTableId      kTableActions = 0xec29063a;
static const ActionContextId    kTableContext = 0xec29063b;
static const MenuContextId      kMenuContext  = 0xec29063c;

class msmaxAction_Window : public ActionItem
{
public:
    int GetId() override { return msmaxActionID_Window; }
    void GetButtonText(MSTR& buttonText) override   { buttonText = MSTR(msmaxMenuTitle_Window); }
    void GetMenuText(MSTR& menuText) override       { menuText = MSTR(msmaxMenuTitle_Window); }
    void GetDescriptionText(MSTR& descText) override{ descText = MSTR(msmaxMenuTitle_Window); }
    void GetCategoryText(MSTR& catText) override    { catText = MSTR(msmaxTitle); }
    BOOL IsChecked() override       { return msmaxGetContext().isSettingWindowOpened(); }
    BOOL IsItemVisible() override   { return TRUE; }
    BOOL IsEnabled() override       { return TRUE; }
    void DeleteThis() override      { delete this; }

    BOOL ExecuteAction() override
    {
        if(msmaxGetContext().isSettingWindowOpened())
            msmaxGetContext().closeSettingWindow();
        else
            msmaxGetContext().openSettingWindow();
        return TRUE;
    }
};

class msmaxAction_Cache : public ActionItem
{
public:
    int GetId() override { return msmaxActionID_Cache; }
    void GetButtonText(MSTR& buttonText) override { buttonText = MSTR(msmaxMenuTitle_Cache); }
    void GetMenuText(MSTR& menuText) override { menuText = MSTR(msmaxMenuTitle_Cache); }
    void GetDescriptionText(MSTR& descText) override { descText = MSTR(msmaxMenuTitle_Cache); }
    void GetCategoryText(MSTR& catText) override { catText = MSTR(msmaxTitle); }
    BOOL IsChecked() override { return msmaxGetContext().isCacheWindowOpened(); }
    BOOL IsItemVisible() override { return TRUE; }
    BOOL IsEnabled() override { return TRUE; }
    void DeleteThis() override { delete this; }

    BOOL ExecuteAction() override
    {
        if (msmaxGetContext().isCacheWindowOpened())
            msmaxGetContext().closeCacheWindow();
        else
            msmaxGetContext().openCacheWindow();
        return TRUE;
    }
};

class msmaxAction_SendScene : public ActionItem
{
public:
    int GetId() override { return msmaxActionID_ExportScene; }
    void GetButtonText(MSTR& buttonText) override   { buttonText = MSTR(msmaxMenuTitle_ExportScene); }
    void GetMenuText(MSTR& menuText) override       { menuText = MSTR(msmaxMenuTitle_ExportScene); }
    void GetDescriptionText(MSTR& descText) override{ descText = MSTR(msmaxMenuTitle_ExportScene); }
    void GetCategoryText(MSTR& catText) override    { catText = MSTR(msmaxTitle); }
    BOOL IsChecked() override       { return FALSE; }
    BOOL IsItemVisible() override   { return TRUE; }
    BOOL IsEnabled() override       { return TRUE; }
    void DeleteThis() override      { delete this; }

    BOOL ExecuteAction() override
    {
        return msmaxSendScene(msmaxExportTarget::Objects, msmaxObjectScope::All);
    }
};

class msmaxAction_SendAnimations : public ActionItem
{
public:
    int GetId() override { return msmaxActionID_ExportAnimations; }
    void GetButtonText(MSTR& buttonText) override   { buttonText = MSTR(msmaxMenuTitle_ExportAnimations); }
    void GetMenuText(MSTR& menuText) override       { menuText = MSTR(msmaxMenuTitle_ExportAnimations); }
    void GetDescriptionText(MSTR& descText) override{ descText = MSTR(msmaxMenuTitle_ExportAnimations); }
    void GetCategoryText(MSTR& catText) override    { catText = MSTR(msmaxTitle); }
    BOOL IsChecked() override       { return FALSE; }
    BOOL IsItemVisible() override   { return TRUE; }
    BOOL IsEnabled() override       { return TRUE; }
    void DeleteThis() override      { delete this; }

    BOOL ExecuteAction() override
    {
        return msmaxSendScene(msmaxExportTarget::Animations, msmaxObjectScope::All);
    }
};

class msmaxAction_Import : public ActionItem
{
public:
    int GetId() override { return msmaxActionID_Import; }
    void GetButtonText(MSTR& buttonText) override   { buttonText = MSTR(msmaxMenuTitle_Import); }
    void GetMenuText(MSTR& menuText) override       { menuText = MSTR(msmaxMenuTitle_Import); }
    void GetDescriptionText(MSTR& descText) override{ descText = MSTR(msmaxMenuTitle_Import); }
    void GetCategoryText(MSTR& catText) override    { catText = MSTR(msmaxTitle); }
    BOOL IsChecked() override       { return FALSE; }
    BOOL IsItemVisible() override   { return TRUE; }
    BOOL IsEnabled() override       { return TRUE; }
    void DeleteThis() override      { delete this; }

    BOOL ExecuteAction() override
    {
        msmaxGetContext().recvScene();
        return TRUE;
    }
};

class msmaxActionCallback : public ActionCallback
{
public:
};
static msmaxActionCallback g_msmaxActionCallback;

void msmaxContext::registerMenu()
{
    unregisterMenu();

    auto *action_manager = GetCOREInterface()->GetActionManager();
    auto *menu_manager = GetCOREInterface()->GetMenuManager();
    {
        auto *table = new ActionTable(kTableActions, kTableContext, TSTR(msmaxTitle));
        table->AppendOperation(new msmaxAction_Window());
        table->AppendOperation(new msmaxAction_Cache());
        table->AppendOperation(new msmaxAction_SendScene());
        table->AppendOperation(new msmaxAction_SendAnimations());
        table->AppendOperation(new msmaxAction_Import());
        action_manager->RegisterActionTable(table);
        action_manager->ActivateActionTable(&g_msmaxActionCallback, kTableActions);
        action_manager->RegisterActionContext(kTableContext, msmaxTitle);
    }

    auto *table = action_manager->FindTable(kTableActions);
    auto *main_menu = menu_manager->GetMainMenuBar();

    bool first_time = menu_manager->RegisterMenuBarContext(kMenuContext, msmaxTitle);
    auto* menu = menu_manager->FindMenu(msmaxTitle);
    if (menu) {
        while (menu->NumItems() > 0)
            menu->RemoveItem(0);
    }
    else {
        menu = GetIMenu();
        menu->SetTitle(msmaxTitle);
        menu_manager->RegisterMenu(menu);
    }

    if (first_time) {
        // add menu to main menu bar if first time
        auto item = GetIMenuItem();
        item->SetActionItem(table->GetAction(msmaxActionID_Window));
        item->SetSubMenu(menu);
        main_menu->AddItem(item);
    }

    {
        auto item = GetIMenuItem();
        item->SetActionItem(table->GetAction(msmaxActionID_Window));
        menu->AddItem(item);
    }
    {
        auto item = GetIMenuItem();
        item->SetActionItem(table->GetAction(msmaxActionID_Cache));
        menu->AddItem(item);
    }
    {
        auto item = GetIMenuItem();
        item->SetActionItem(table->GetAction(msmaxActionID_ExportScene));
        menu->AddItem(item);
    }
    {
        auto item = GetIMenuItem();
        item->SetActionItem(table->GetAction(msmaxActionID_ExportAnimations));
        menu->AddItem(item);
    }
    //{
    //    auto item = GetIMenuItem();
    //    item->SetActionItem(table->GetAction(msmaxActionID_Import));
    //    menu->AddItem(item);
    //}
}


void msmaxContext::unregisterMenu()
{
    // nothing to do for now
}


extern HINSTANCE g_msmax_hinstance;
HWND g_msmax_settings_window;
HWND g_msmax_cache_window;

HWND g_msmax_current_window;

static bool CtrlIsChecked(int cid)
{
    return IsDlgButtonChecked(g_msmax_current_window, cid);
};
static void CtrlSetCheck(int cid, bool v)
{
    CheckDlgButton(g_msmax_current_window, cid, v);
};

static int CtrlGetInt(int cid, int default_value, bool sign = true)
{
    BOOL valid;
    int v = GetDlgItemInt(g_msmax_current_window, cid, &valid, sign);
    return valid ? v : default_value;
};

static float CtrlGetFloat(int cid, float default_value)
{
    BOOL valid;
    float v = GetDlgItemFloat(g_msmax_current_window, cid, &valid);
    return valid ? v : default_value;
};


static std::string CtrlGetText(int cid)
{
    wchar_t buf[256];
    GetDlgItemText(g_msmax_current_window, IDC_EDIT_SERVER, buf, _countof(buf));
    return mu::ToMBS(buf);
}
static void CtrlSetText(int cid, const std::string& v)
{
    SetDlgItemTextA(g_msmax_current_window, cid, v.c_str());
}
static void CtrlSetText(int cid, int v)
{
    wchar_t buf[256];
    swprintf(buf, L"%d", v);
    SetDlgItemText(g_msmax_current_window, cid, buf);

}
static void CtrlSetText(int cid, float v)
{
    wchar_t buf[256];
    swprintf(buf, L"%.3f", v);
    SetDlgItemText(g_msmax_current_window, cid, buf);
}
static void CtrlAddString(int cid, const std::string& v)
{
    SendDlgItemMessageA(g_msmax_current_window, cid, CB_ADDSTRING, 0, (LPARAM)v.c_str());
   
}

static void PositionWindowNearCursor(HWND hwnd)
{
    RECT rect;
    POINT cur;
    GetWindowRect(hwnd, &rect);
    GetCursorPos(&cur);
    int scrwid = GetScreenWidth();
    int scrhgt = GetScreenHeight();
    int winwid = rect.right - rect.left;
    int winhgt = rect.bottom - rect.top;
    cur.x -= winwid / 3; // 1/3 of the window should be to the left of the cursor
    cur.y -= winhgt / 3; // 1/3 of the window should be above the cursor
    if (cur.x + winwid > scrwid - 10) cur.x = scrwid - winwid - 10; // check too far right 
    if (cur.x < 10) cur.x = 10; // check too far left
    if (cur.y + winhgt > scrhgt - 50) cur.y = scrhgt - 50 - winhgt; // check if too low
    if (cur.y < 10) cur.y = 10; // check for too high s45
    MoveWindow(hwnd, cur.x, cur.y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

static INT_PTR CALLBACK msmaxSettingWindowCB(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // see this about DisableAccelerators(), EnableAccelerators() and GetCOREInterface()->RegisterDlgWnd()
    // https://help.autodesk.com/view/3DSMAX/2018/ENU/?guid=__developer_3ds_max_sdk_features_user_interface_action_system_keyboard_accelerators_and_dialog_html

    auto& ctx = msmaxGetContext();
    auto& s = msmaxGetSettings();
    g_msmax_current_window = hDlg;

    INT_PTR ret = FALSE;
    switch (msg) {
    case WM_INITDIALOG:
        ret = TRUE;
        g_msmax_settings_window = hDlg;
        GetCOREInterface()->RegisterDlgWnd(g_msmax_settings_window);
        PositionWindowNearCursor(hDlg);
        ctx.updateSettingControls();
        ShowWindow(hDlg, SW_SHOW);
        break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        break;

    case WM_DESTROY:
        GetCOREInterface()->UnRegisterDlgWnd(g_msmax_settings_window);
        g_msmax_settings_window = nullptr;
        break;

    case WM_COMMAND:
    {
        int code = HIWORD(wParam);
        int cid = LOWORD(wParam);

        auto handle_edit = [&](const std::function<void()>& body) {
            switch (code) {
            case EN_SETFOCUS:
                DisableAccelerators();
                ret = TRUE;
                break;
            case EN_KILLFOCUS:
                EnableAccelerators();
                ret = TRUE;
                break;
            case EN_CHANGE:
                ret = TRUE;
                body();
                break;
            }
        };

        auto handle_button = [&](const std::function<void()>& body) {
            switch (code) {
            case BN_CLICKED:
                body();
                ret = TRUE;
                break;
            }
        };

        auto notify_scene_update = []() {
            auto& ctx = msmaxGetContext();
            ctx.addDeferredCall([&ctx]() {
                ctx.onSceneUpdated();
                ctx.update();
            });
        };

        switch (cid) {
        case IDC_EDIT_SERVER:
            handle_edit([&]() { s.client_settings.server = CtrlGetText(IDC_EDIT_SERVER); });
            break;
        case IDC_EDIT_PORT:
            handle_edit([&]() {
                int tmp = CtrlGetInt(IDC_EDIT_PORT, s.client_settings.port);
                if (tmp < 0 || tmp > 63335) {
                    tmp = mu::clamp((int)tmp, 0, 63335);
                    CtrlSetText(IDC_EDIT_PORT, tmp);
                }
                s.client_settings.port = tmp;
            });
            break;
        case IDC_EDIT_SCALE_FACTOR:
            handle_edit([&]() {
                s.scale_factor = CtrlGetFloat(IDC_EDIT_SCALE_FACTOR, s.scale_factor);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_MESHES:
            handle_button([&]() {
                s.sync_meshes = CtrlIsChecked(IDC_CHECK_MESHES);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_MAKE_DOUBLE_SIDED:
            handle_button([&]() {
                s.make_double_sided = CtrlIsChecked(IDC_CHECK_MAKE_DOUBLE_SIDED);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_IGNORE_NON_RENDERABLE:
            handle_button([&]() {
                s.ignore_non_renderable = CtrlIsChecked(IDC_CHECK_IGNORE_NON_RENDERABLE);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_BAKE_MODIFIERS:
            handle_button([&]() {
                s.bake_modifiers = CtrlIsChecked(IDC_CHECK_BAKE_MODIFIERS);
                if (!s.bake_modifiers && CtrlIsChecked(IDC_CHECK_USE_RENDER_MESHES)) {
                    s.use_render_meshes = false;
                    CtrlSetCheck(IDC_CHECK_USE_RENDER_MESHES, false);
                }
                notify_scene_update();
            });
            break;
        case IDC_CHECK_USE_RENDER_MESHES:
            handle_button([&]() {
                if (CtrlIsChecked(IDC_CHECK_USE_RENDER_MESHES)) {
                    if (!s.bake_modifiers) {
                        s.bake_modifiers = true;
                        CtrlSetCheck(IDC_CHECK_BAKE_MODIFIERS, true);
                    }
                    s.use_render_meshes = true;
                }
                else {
                    s.use_render_meshes = false;
                }
                notify_scene_update();
            });
            break;
        case IDC_CHECK_BLENDSHAPES:
            handle_button([&]() {
                s.sync_blendshapes = CtrlIsChecked(IDC_CHECK_BLENDSHAPES);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_BONES:
            handle_button([&]() {
                s.sync_bones = CtrlIsChecked(IDC_CHECK_BONES);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_TEXTURES:
            handle_button([&]() {
                s.sync_textures = CtrlIsChecked(IDC_CHECK_TEXTURES);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_CAMERAS:
            handle_button([&]() {
                s.sync_cameras = CtrlIsChecked(IDC_CHECK_CAMERAS);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_LIGHTS:
            handle_button([&]() {
                s.sync_lights = CtrlIsChecked(IDC_CHECK_LIGHTS);
                notify_scene_update();
            });
            break;
        case IDC_CHECK_AUTO_SYNC:
            handle_button([&]() {
                auto& ctx = msmaxGetContext();
                auto& settings = msmaxGetSettings();
                if (CtrlIsChecked(IDC_CHECK_AUTO_SYNC)) {
                    if (ctx.isServerAvailable()) {
                        settings.auto_sync = true;
                        notify_scene_update();
                    }
                    else {
                        ctx.logInfo("MeshSync: Server not available. %s", ctx.getErrorMessage().c_str());
                        CtrlSetCheck(IDC_CHECK_AUTO_SYNC, false);
                    }
                }
                else {
                    settings.auto_sync = false;
                }
            });
            break;

        case IDC_EDIT_ANIMATION_TIME_SCALE:
            handle_edit([&]() {
                float tmp = CtrlGetFloat(IDC_EDIT_ANIMATION_TIME_SCALE, s.animation_time_scale);
                if (tmp < 0.01f) {
                    tmp = mu::max(tmp, 0.01f);
                    CtrlSetText(IDC_EDIT_ANIMATION_TIME_SCALE, tmp);
                }
                s.animation_time_scale = tmp;
            });
            break;
        case IDC_EDIT_ANIMATION_SPS:
            handle_edit([&]() {
                float tmp = CtrlGetFloat(IDC_EDIT_ANIMATION_SPS, s.animation_sps);
                if (tmp < 0.01f) {
                    tmp = mu::max(tmp, 0.01f);
                    CtrlSetText(IDC_EDIT_ANIMATION_SPS, tmp);
                }
                s.animation_sps = tmp;
            });
            break;
        case IDC_CHECK_KFREDUCTION:
            handle_button([&]() {
                s.keyframe_reduction = CtrlIsChecked(IDC_CHECK_KFREDUCTION);
            });
            break;
        case IDC_CHECK_KEEPFLATCURVES:
            handle_button([&]() {
                s.keep_flat_curves = CtrlIsChecked(IDC_CHECK_KEEPFLATCURVES);
            });
            break;
        case IDC_BUTTON_MANUAL_SYNC:
            handle_button([&]() { msmaxSendScene(msmaxExportTarget::Objects, msmaxObjectScope::All); });
            break;
        case IDC_BUTTON_SYNC_ANIMATIONS:
            handle_button([&]() { msmaxSendScene(msmaxExportTarget::Animations, msmaxObjectScope::All); });
            break;

        case IDC_BUTTON_EXPORT_CACHE:
            handle_button([&]() { msmaxExportCache(msmaxObjectScope::All, false); });
            break;
        case IDC_BUTTON_EXPORT_CACHE_ALL:
            handle_button([&]() { msmaxExportCache(msmaxObjectScope::All, true); });
            break;
        default: break;
        }
        break;
    }

    default:
        break;
    }
    return ret;
}

void msmaxContext::openSettingWindow()
{
    if (!g_msmax_settings_window) {
        CreateDialogParam(g_msmax_hinstance, MAKEINTRESOURCE(IDD_SETTINGS_WINDOW),
            GetCOREInterface()->GetMAXHWnd(), msmaxSettingWindowCB, (LPARAM)this);
    }
}

void msmaxContext::closeSettingWindow()
{
    if (g_msmax_settings_window) {
        PostMessage(g_msmax_settings_window, WM_CLOSE, 0, 0);
    }
}

bool msmaxContext::isSettingWindowOpened() const
{
    return g_msmax_settings_window != nullptr;
}

void msmaxContext::updateSettingControls()
{
    auto& s = m_settings;
    CtrlSetText(IDC_EDIT_SERVER, s.client_settings.server);
    CtrlSetText(IDC_EDIT_PORT, (int)s.client_settings.port);

    CtrlSetText(IDC_EDIT_SCALE_FACTOR,      s.scale_factor);
    CtrlSetCheck(IDC_CHECK_MESHES,                  s.sync_meshes);
    CtrlSetCheck(IDC_CHECK_MAKE_DOUBLE_SIDED,       s.make_double_sided);
    CtrlSetCheck(IDC_CHECK_IGNORE_NON_RENDERABLE,   s.ignore_non_renderable);
    CtrlSetCheck(IDC_CHECK_BAKE_MODIFIERS,          s.bake_modifiers);
    CtrlSetCheck(IDC_CHECK_USE_RENDER_MESHES,       s.use_render_meshes);
    CtrlSetCheck(IDC_CHECK_BLENDSHAPES,     s.sync_blendshapes);
    CtrlSetCheck(IDC_CHECK_BONES,           s.sync_bones);
    CtrlSetCheck(IDC_CHECK_TEXTURES,        s.sync_textures);
    CtrlSetCheck(IDC_CHECK_CAMERAS,         s.sync_cameras);
    CtrlSetCheck(IDC_CHECK_LIGHTS,          s.sync_lights);
    CtrlSetCheck(IDC_CHECK_AUTO_SYNC,       s.auto_sync);

    CtrlSetText(IDC_EDIT_ANIMATION_TIME_SCALE, s.animation_time_scale);
    CtrlSetText(IDC_EDIT_ANIMATION_SPS,        s.animation_sps);
    CtrlSetCheck(IDC_CHECK_KFREDUCTION,        s.keyframe_reduction);

    CtrlSetText(IDC_TXT_VERSION, "Plugin Version: " msPluginVersionStr);
}



static INT_PTR CALLBACK msmaxCacheWindowCB(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto& ctx = msmaxGetContext();
    auto& s = msmaxGetCacheSettings();
    g_msmax_current_window = hDlg;

    INT_PTR ret = FALSE;
    switch (msg) {
    case WM_INITDIALOG:
        ret = TRUE;
        g_msmax_cache_window = hDlg;

        // init combo boxes
        CtrlAddString(IDC_OBJECT_SCOPE, "All");
        CtrlAddString(IDC_OBJECT_SCOPE, "Selected");

        GetCOREInterface()->RegisterDlgWnd(g_msmax_cache_window);
        PositionWindowNearCursor(hDlg);
        ctx.updateCacheControls();

        ShowWindow(hDlg, SW_SHOW);
        break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        break;

    case WM_DESTROY:
        GetCOREInterface()->UnRegisterDlgWnd(g_msmax_cache_window);
        g_msmax_cache_window = nullptr;
        break;

    case WM_COMMAND:
    {
        int code = HIWORD(wParam);
        int cid = LOWORD(wParam);

        auto handle_edit = [&](const std::function<void()>& body) {
            switch (code) {
            case EN_SETFOCUS:
                DisableAccelerators();
                ret = TRUE;
                break;
            case EN_KILLFOCUS:
                EnableAccelerators();
                ret = TRUE;
                break;
            case EN_CHANGE:
                ret = TRUE;
                body();
                break;
            }
        };

        auto handle_button = [&](const std::function<void()>& body) {
            switch (code) {
            case BN_CLICKED:
                body();
                ret = TRUE;
                break;
            }
        };

        auto notify_scene_update = []() {
            auto& ctx = msmaxGetContext();
            ctx.addDeferredCall([&ctx]() {
                ctx.onSceneUpdated();
                ctx.update();
            });
        };

        switch (cid) {
        default: break;
        }
        break;
    }

    default:
        break;
    }
    return ret;
}

void msmaxContext::openCacheWindow()
{
    if (!g_msmax_cache_window) {
        CreateDialogParam(g_msmax_hinstance, MAKEINTRESOURCE(IDD_CACHE_WINDOW),
            GetCOREInterface()->GetMAXHWnd(), msmaxCacheWindowCB, (LPARAM)this);
    }
}

void msmaxContext::closeCacheWindow()
{
    if (g_msmax_cache_window) {
        PostMessage(g_msmax_cache_window, WM_CLOSE, 0, 0);
    }
}

bool msmaxContext::isCacheWindowOpened() const
{
    return g_msmax_cache_window != nullptr;
}

void msmaxContext::updateCacheControls()
{
}
