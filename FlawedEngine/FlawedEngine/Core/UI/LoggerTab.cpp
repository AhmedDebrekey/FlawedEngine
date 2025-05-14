#include "../Logger.h"
#include "../UIManager.h"

static std::string FormatTimestamp(const std::chrono::system_clock::time_point& tp)
{
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&time);

    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm); // e.g., 21:37:45
    return std::string(buffer);
}


void FlawedEngine::cUIManager::RenderLogger()
{
    ImGui::Begin("Log");

    static bool autoScroll = true;
    ImGui::Checkbox("Auto-scroll", &autoScroll);
    ImGui::SameLine();
    if (ImGui::Button("Clear Logs", ImVec2{ 100, 30 }))
    {
        Logger::Clear();
    }
    ImGui::Separator();

    const auto& logs = Logger::GetLogs();
    bool skip = false;

    for (const auto& entry : logs)
    {
        // Format timestamp
        std::string timeStr = '[' + FormatTimestamp(entry.Timestamp) + ']';

        // Select color and label for log level
        ImVec4 color;
        std::string levelTag;

        switch (entry.Level)
        {
        case LogLevel::Info:
            color = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);  // Light blue
            levelTag = "[INFO]";
            break;
        case LogLevel::Warning:
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
            levelTag = "[WARNING]";
            break;
        case LogLevel::Error:
            color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);  // Red
            levelTag = "[ERROR]";
            break;
        case LogLevel::Script:
            color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
            levelTag = "[Script]";
            break;
        case LogLevel::Console:
            skip = true;
            break;
        }

        // Compose full message
        if (skip)
            continue;
        ImGui::TextUnformatted(timeStr.c_str());
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(levelTag.c_str());
        ImGui::PopStyleColor();
        ImGui::TextWrapped("%s", entry.Message.c_str());
        ImGui::Separator();
    }


    // Auto-scroll
    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f)
        ImGui::SetScrollHereY(1.0f); // Scroll to bottom

    ImGui::End();
}


