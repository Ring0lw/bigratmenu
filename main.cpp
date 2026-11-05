#include <cstdint>
#include <cstdio>
#include <filesystem>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include "stb_image.h"

namespace LW::menu {

struct Settings {
    bool esp = true;
    bool boxes = true;
    bool names = false;
    int fov = 90;
    float boxcolor[4] = {1.0f, 0.25f, 0.25f, 1.0f};
    bool watermark = true;
    bool showfps = true;
};

struct Texture {
    GLuint id = 0;
    int w = 0;
    int h = 0;
};

constexpr ImGuiKey menukey = ImGuiKey_A;

Texture loadtexture(const std::filesystem::path& path) {
    Texture t;
    unsigned char* px = stbi_load(path.c_str(), &t.w, &t.h, nullptr, 4);
    if (!px) {
        return t;
    }
    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.w, t.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    stbi_image_free(px);
    return t;
}

void drawlogo(const Texture& rat) {
    if (!rat.id) {
        return;
    }
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    float h = size.y - ImGui::GetFrameHeight();
    float w = h * rat.w / rat.h;
    ImVec2 max(pos.x + size.x, pos.y + size.y);
    ImVec2 min(max.x - w, max.y - h);
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)rat.id, min, max, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), IM_COL32_WHITE);
}

void drawvisuals(Settings& s) {
    ImGui::Checkbox("esp", &s.esp);
    ImGui::Checkbox("big box", &s.boxes);
    ImGui::SameLine(160.0f);
    ImGui::Checkbox("name", &s.names);
    ImGui::SliderInt("FOV", &s.fov, 60, 120);
    ImGui::ColorEdit4("bigratbox color", s.boxcolor, ImGuiColorEditFlags_NoInputs);
}

void drawaimbot() {
    ImGui::TextUnformatted("z");
}

void drawmisc(Settings& s) {
    ImGui::Checkbox("h20mark", &s.watermark);
    ImGui::Checkbox("show fps", &s.showfps);
    ImGui::LabelText("menu key", "a");
}

bool drawmenu(Settings& s, const Texture& rat) {
    ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380.0f, 0.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("imgui window", nullptr, ImGuiWindowFlags_NoCollapse);
    drawlogo(rat);

    if (ImGui::BeginTabBar("tabs")) {
        if (ImGui::BeginTabItem("ESP")) {
            drawvisuals(s);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("aim")) {
            drawaimbot();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("various shit")) {
            drawmisc(s);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Separator();
    bool unload = ImGui::Button("unload");
    ImGui::End();
    return unload;
}

void drawwatermark(const Settings& s) {
    const char* text;
    if (s.showfps) {
        text = "SKIDDING";
    } else if (s.watermark) {
        text = "EEEEEEE";
    } else {
        return;
    }

    ImVec2 size = ImGui::CalcTextSize(text);
    ImVec2 pos(ImGui::GetIO().DisplaySize.x - size.x - 14.0f, 12.0f);
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    dl->AddRectFilled(ImVec2(pos.x - 7.0f, pos.y - 4.0f), ImVec2(pos.x + size.x + 7.0f, pos.y + size.y + 4.0f), IM_COL32(0, 0, 0, 170), 4.0f);
    dl->AddText(pos, IM_COL32_WHITE, text);
}

} // namespace LW::menu

int main() {
    if (!glfwInit()) {
        std::puts("[!] AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "imgui window", nullptr, nullptr);
    if (!window) {
        std::puts("[!] AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::error_code ec;
    std::filesystem::path exe = std::filesystem::read_symlink("/proc/self/exe", ec);
    LW::menu::Texture rat = LW::menu::loadtexture(exe.parent_path() / "bigrat.jpg");

    std::puts("[+] menu open");

    LW::menu::Settings s;
    bool visible = true;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::IsKeyPressed(LW::menu::menukey, false)) {
            visible = !visible;
        }
        if (visible && LW::menu::drawmenu(s, rat)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        LW::menu::drawwatermark(s);

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.07f, 0.07f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &rat.id);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::puts("[+] menu closed");
}
