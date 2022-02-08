#pragma once


#include "imgui.h"
#include "Gameplay/Window.h"

class GameViewWindow
{
public:
	static void imgui(bool* p_open) {
		if (ImGui::Begin("Game Viewport", p_open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar()) {
				if (ImGui::MenuItem("Play", "", Editor::IsPlaying, !Editor::IsPlaying)) {
					Editor::IsPlaying = true;
				} 
				if (ImGui::MenuItem("Stop", "", !Editor::IsPlaying, Editor::IsPlaying)) {
					Editor::IsPlaying = false;
				}

				ImGui::EndMenuBar();
			}

			ImGui::SetCursorPos(ImGui::GetCursorPos());
			ImVec2 windowSize = getLargestSizeForViewport();
			ImVec2 windowPos = getCenteredPositionForViewport(windowSize);
			ImGui::SetCursorPos(windowPos);


			GLuint textureId = Window::GetFramebuffer().GetTextureID();
			ImGui::Image(&textureId, windowSize);

			ImGui::End();
		}		
	}

private:

	static ImVec2 getLargestSizeForViewport() {
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		windowSize.x = ImGui::GetScrollX();
		windowSize.y = ImGui::GetScrollY();

		float aspectWidth = windowSize.x;
		float aspectHeight = aspectWidth / Window::GetTargetAspectRatio();
		if (aspectHeight > windowSize.y) {
			aspectHeight = windowSize.y;
			aspectWidth = aspectHeight * Window::GetTargetAspectRatio();
		}
		
		return ImVec2(aspectWidth, aspectHeight);
	}

	static ImVec2 getCenteredPositionForViewport(ImVec2 aspectSize) {
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		windowSize.x = ImGui::GetScrollX();
		windowSize.y = ImGui::GetScrollY();

		float viewportX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
		float viewportY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

		return ImVec2(viewportX + ImGui::GetCursorPosX(), viewportY + ImGui::GetCursorPosY());
	}
};

