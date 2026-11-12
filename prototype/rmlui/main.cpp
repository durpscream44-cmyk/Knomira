// Isolated RmlUi evaluation executable for Knomira.
// Does not depend on Ingnomia simulation, Steam, or NoesisGUI.
#include <RmlUi/Core.h>
#include <RmlUi_Backend.h>

#include <filesystem>
#include <iostream>

namespace {

class MenuEvents final : public Rml::EventListener {
public:
    explicit MenuEvents(Rml::ElementDocument* document) : document_(document) {}

    void ProcessEvent(Rml::Event& event) override {
        Rml::Element* element = event.GetCurrentElement();
        if (!element) return;
        const Rml::String id = element->GetId();
        if (id == "quit") {
            Backend::RequestExit();
        } else if (id == "new_game") {
            SetStatus("New Game selected. World generation is not connected yet.");
        } else if (id == "load_game") {
            SetStatus("Load Game selected. Save discovery is not connected yet.");
        } else if (id == "settings") {
            SetStatus("Settings selected. Game preferences are not connected yet.");
        }
    }

private:
    void SetStatus(const char* message) {
        if (Rml::Element* status = document_->GetElementById("status"))
            status->SetInnerRML(message);
    }

    Rml::ElementDocument* document_;
};

bool LoadFont() {
#ifdef _WIN32
    const char* candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
    };
#else
    const char* candidates[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
    };
#endif
    for (const char* candidate : candidates) {
        if (std::filesystem::exists(candidate) && Rml::LoadFontFace(candidate))
            return true;
    }
    return false;
}

} // namespace

int main(int argc, char** argv) {
    constexpr int width = 1280;
    constexpr int height = 800;
    if (!Backend::Initialize("Knomira | RmlUi Prototype", width, height, true)) {
        std::cerr << "Failed to initialize GLFW/OpenGL3 UI backend.\n";
        return 1;
    }

    Rml::SetSystemInterface(Backend::GetSystemInterface());
    Rml::SetRenderInterface(Backend::GetRenderInterface());
    if (!Rml::Initialise()) {
        Backend::Shutdown();
        std::cerr << "RmlUi initialization failed.\n";
        return 1;
    }

    Rml::Context* context = Rml::CreateContext("main", Rml::Vector2i(width, height));
    if (!context || !LoadFont()) {
        std::cerr << "Cannot create context or load a TrueType font.\n";
        Rml::Shutdown();
        Backend::Shutdown();
        return 1;
    }

    // Use the executable's directory instead of the CI machine's source path.
    // CMake copies main_menu.rml next to this executable after building.
    const std::filesystem::path executable = std::filesystem::absolute(
        argc > 0 && argv[0] && argv[0][0] ? argv[0] : "knomira_rmlui_prototype");
    const std::filesystem::path menu_file = executable.parent_path() / "main_menu.rml";
    const Rml::String menu_path = menu_file.generic_string();
    Rml::ElementDocument* document = context->LoadDocument(menu_path);
    if (!document) {
        std::cerr << "Failed to load menu: " << menu_path << "\n";
        Rml::Shutdown();
        Backend::Shutdown();
        return 1;
    }

    MenuEvents events(document);
    for (const char* id : {"new_game", "load_game", "settings", "quit"}) {
        if (Rml::Element* element = document->GetElementById(id))
            element->AddEventListener("click", &events);
    }
    document->Show();

    while (Backend::ProcessEvents(context, nullptr, true)) {
        context->Update();
        Backend::BeginFrame();
        context->Render();
        Backend::PresentFrame();
    }

    Rml::Shutdown();
    Backend::Shutdown();
    return 0;
}
