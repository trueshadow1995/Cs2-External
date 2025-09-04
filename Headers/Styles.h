class Styles {
 public:
  static void Apply() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();

 
    static bool font_loaded = false;
    if (!font_loaded) {
      io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Bahnschrift.ttf",
                                   15.0f);
      font_loaded = true;
    }

    // permanent style values
    style.WindowRounding = 6.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowPadding = ImVec2(6, 30);
    style.ItemSpacing = ImVec2(4, 3);
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.FramePadding = ImVec2(6, 3);
    style.ChildBorderSize = 0.0f;

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.03f, 0.06f, 0.95f);
    style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 0.95f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.10f, 0.5f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.08f, 0.95f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.12f, 0.95f);

    ImGui::SetNextWindowSize({850, 650}, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.95f);
  }
};
