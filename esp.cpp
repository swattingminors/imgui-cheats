#include "esp.hpp"

#include "../../utils/overlay/imgui/imgui.h"
#include "../globals/globals.hpp"

#include <Windows.h>


float calc_dist_3d(antagonist::roblox::vector3_t first, antagonist::roblox::vector3_t sec)
{
	return sqrt((first.x - sec.x) * (first.x - sec.x) + (first.y - sec.y) * (first.y - sec.y) + (first.z - sec.z) * (first.z - sec.z));
}

antagonist::roblox::vector3_t vector_sub(antagonist::roblox::vector3_t one, antagonist::roblox::vector3_t two)
{
	return { one.x - two.x, one.y - two.y, one.z - two.z };
}

float vector3_mag(antagonist::roblox::vector3_t vector)
{
    return sqrtf((vector.x * vector.y) + (vector.y * vector.y) + (vector.z * vector.z));
}

void DrawEsp(int X, int Y, int W, int H, ImColor Color) {
	float lineW = (W / 5);
	float lineH = (H / 6);
	float lineT = 1;

	// Outline
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X - lineT, Y - lineT), ImVec2(X + lineW, Y - lineT), ImColor(0, 0, 0), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X - lineT, Y - lineT), ImVec2(X - lineT, Y + lineH), ImColor(0, 0, 0), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X - lineT, Y + H - lineH), ImVec2(X - lineT, Y + H + lineT), ImColor(0, 0, 0), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X - lineT, Y + H + lineT), ImVec2(X + lineW, Y + H + lineT), ImColor(0, 0, 0), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W - lineW, Y - lineT), ImVec2(X + W + lineT, Y - lineT), ImColor(0, 0, 0), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W + lineT, Y - lineT), ImVec2(X + W + lineT, Y + lineH), ImColor(0, 0, 0), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W + lineT, Y + H - lineH), ImVec2(X + W + lineT, Y + H + lineT), ImColor(0, 0, 0), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W - lineW, Y + H + lineT), ImVec2(X + W + lineT, Y + H + lineT), ImColor(0, 0, 0), lineT);

	// Inline
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImColor(255, 255, 255), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImColor(255, 255, 255), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImColor(255, 255, 255), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImColor(255, 255, 255), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImColor(255, 255, 255), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImColor(255, 255, 255), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImColor(255, 255, 255), lineT);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImColor(255, 255, 255), lineT);
}

void antagonist::roblox::hook_esp()
{
    auto players = globals::players;
    auto draw = ImGui::GetBackgroundDrawList();

    POINT cursor_pos;
    GetCursorPos(&cursor_pos);
    ScreenToClient(FindWindowA(0, "Roblox"), &cursor_pos);

    antagonist::roblox::vector3_t camerapos = globals::visualengine.get_camera_pos();

    if (globals::fov_on)
    {
        draw->AddCircle(ImVec2(cursor_pos.x, cursor_pos.y), globals::fov, ImColor(255, 255, 255));
    }

    if (globals::esp)
    {
        for (auto player : players.children())
        {
            // localplayer check
            auto localplayer = players.get_local_player();
            if (player.self == localplayer.self)
                continue;

            auto character = player.get_model_instance();
            if (!character.self)
                continue;

            auto humanoid = character.find_first_child("Humanoid");
            if (!humanoid.self)
                continue;

            auto head = character.find_first_child("Head");
            if (!head.self)
                continue;

            auto hrp = character.find_first_child("HumanoidRootPart");
            if (!hrp.self)
                continue;

            auto team = player.get_team();
           
            auto localplayer_team = localplayer.get_team();

            if (globals::team_check_esp && (team.self == localplayer_team.self))
                continue;

            if (humanoid.get_health() <= 0 && globals::healthcheck)
            {
                continue;
            }

            if (humanoid.get_health() <= 4 && globals::knock_check)
            {
                continue;
            }
            
            auto head_position_3d = head.get_part_pos();
            auto head_position = antagonist::roblox::world_to_screen(vector_sub(head_position_3d, { 0, 2, 0 }));

            auto hrp_pos = hrp.get_part_pos();

            auto leg_pos = antagonist::roblox::world_to_screen(vector_sub(head.get_part_pos(), { 0, 5, 0 }));
            if (leg_pos.x == -1)
                continue;

            float distance_from_camera = vector3_mag(vector_sub(camerapos, hrp_pos));

            float height = head_position.y - leg_pos.y;
            float width = height / 1.6f;

            // tracers -> line
            if (globals::tracers)
            {
                draw->AddLine(ImVec2(head_position.x, head_position.y), ImVec2(static_cast<float>(cursor_pos.x), static_cast<float>(cursor_pos.y)), ImColor(255, 255, 255));
            }

            // name esp -> text
            if (globals::name_esp)
            {
                std::string playerName = player.name();

                draw->AddText(ImVec2(head_position.x, head_position.y) + ImVec2(-1, -1), ImColor(0, 0, 0), playerName.c_str());
                draw->AddText(ImVec2(head_position.x, head_position.y) + ImVec2(1, -1), ImColor(0, 0, 0), playerName.c_str());
                draw->AddText(ImVec2(head_position.x, head_position.y) + ImVec2(-1, 1), ImColor(0, 0, 0), playerName.c_str());
                draw->AddText(ImVec2(head_position.x, head_position.y) + ImVec2(1, 1), ImColor(0, 0, 0), playerName.c_str());

                draw->AddText(ImVec2(head_position.x, head_position.y), ImColor(255, 255, 255), playerName.c_str());
            }

            if (globals::healthbar)
            {
            //nigger kys
            }

            //health info -> text
            if (globals::healthinfo)
            {
                auto humanoid = character.find_first_child("Humanoid");
                if (humanoid.self)
                {
                    float health = humanoid.get_health();
                    int healthPercentage = static_cast<int>(health);

                    ImVec4 healthColor;

                    if (healthPercentage <= 100)
                        healthColor = ImColor(66, 245, 164);

                    if (healthPercentage <= 90)
                        healthColor = ImColor(66, 245, 111);

                    if (healthPercentage <= 70)
                        healthColor = ImColor(239, 245, 66);

                    if (healthPercentage <= 60)
                        healthColor = ImColor(245, 215, 66);

                    if (healthPercentage <= 50)
                        healthColor = ImColor(245, 176, 66);

                    if (healthPercentage <= 40)
                        healthColor = ImColor(245, 150, 66);

                    if (healthPercentage <= 30)
                        healthColor = ImColor(245, 123, 66);

                    if (healthPercentage <= 20)
                        healthColor = ImColor(245, 102, 66);

                    if (healthPercentage <= 10)
                        healthColor = ImColor(245, 66, 66);

                    std::string text = std::to_string(healthPercentage) + "%";

                    draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(-1, -1), ImColor(0, 0, 0), text.c_str());
                    draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(1, -1), ImColor(0, 0, 0), text.c_str());
                    draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(-1, 1), ImColor(0, 0, 0), text.c_str());
                    draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(1, 1), ImColor(0, 0, 0), text.c_str());

                    draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2), (ImColor)healthColor, text.c_str());
                }
            }

            // box -> rectangle & circle (???)
            if (globals::box)
            {
                switch (globals::boxtype)
                {
                case 0:
                    draw->AddRect(ImVec2(leg_pos.x - (width / 1.3f), leg_pos.y), ImVec2(head_position.x + width, head_position.y + height), ImColor(255, 255, 255, 255));
                    break;
                case 1:
                    DrawEsp(leg_pos.x - (width / 1.3f), leg_pos.y, head_position.x + width - leg_pos.x + (width / 1.3f), head_position.y + height - leg_pos.y, ImColor(255, 255, 255));
                    break;
                case 2:
                    draw->AddCircle(ImVec2((leg_pos.x + head_position.x + width) / 2, (leg_pos.y + head_position.y + height) / 2), sqrt(pow((head_position.x + width - leg_pos.x + (width / 1.3f)) / 2, 2) + pow((head_position.y + height - leg_pos.y) / 2, 2)), ImColor(255, 255, 255, 255), 30);
                    break;
                }
            }
        }
    }
}