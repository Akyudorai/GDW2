#pragma once

#include "Gameplay/GameObject.h"
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"

#include <string.h>

#include "Gameplay/Scene.h"

#include "Graphics/Textures/Texture2D.h"
#include "Application/Application.h"

using namespace Gameplay;

class UIHelper
{
public:
	static GameObject::Sptr CreateButton(Scene::Sptr scene, std::string name)
	{
		std::string objectName = (name + " Button");
		GameObject::Sptr buttonObject = scene->CreateGameObject(objectName);
		{
			RectTransform::Sptr transform = buttonObject->Add<RectTransform>();
			transform->SetMin({ 15, 15 });
			transform->SetMax({ 300, 50 });

			GuiPanel::Sptr image = buttonObject->Add<GuiPanel>();
			image->SetColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
		}

		GameObject::Sptr buttonTextObject = scene->CreateGameObject("Text Component");
		{
			RectTransform::Sptr transform = buttonTextObject->Add<RectTransform>();
			transform->SetMin({ 15,15 });
			transform->SetMax({ 300,75 });
			transform->SetPosition({ 280, 52.5 });

			Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
			font->Bake();

			GuiText::Sptr text = buttonTextObject->Add<GuiText>();
			text->SetText(name);
			text->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			text->SetFont(font);
		}

		buttonObject->AddChild(buttonTextObject);

		return buttonObject;
	}

	static GameObject::Sptr CreateText(Scene::Sptr scene, std::string _text, std::string name = "New Text")
	{
		GameObject::Sptr textObject = scene->CreateGameObject(name);
		{
			RectTransform::Sptr transform = textObject->Add<RectTransform>();
			transform->SetMin({ 15, 15 });
			transform->SetMax({ 300, 50 });

			Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
			font->Bake();

			GuiText::Sptr text = textObject->Add<GuiText>();
			text->SetText(_text);
			text->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			text->SetFont(font);

		}

		return textObject;
	}

	static GameObject::Sptr CreateText1(Scene::Sptr scene, std::string _text, std::string name = "New Text")
	{		
		GameObject::Sptr textObject1 = scene->CreateGameObject(name);
		{
			RectTransform::Sptr transform = textObject1->Add<RectTransform>();
			transform->SetMin({ 15, 15 });
			transform->SetMax({ 300, 50 });

			Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
			font->Bake();

			GuiText::Sptr text = textObject1->Add<GuiText>();
			text->SetText(_text);
			text->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
			text->SetFont(font);
		}

		return textObject1;
	}

	static GameObject::Sptr CreateImage(Scene::Sptr scene, Texture2D::Sptr texture, std::string name = "Image")
	{		
		GameObject::Sptr imageObject = scene->CreateGameObject(name);
		{
			RectTransform::Sptr transform = imageObject->Add<RectTransform>();
			transform->SetMin({ 15, 15 });
			transform->SetMax({ 256,256 });

			GuiPanel::Sptr image = imageObject->Add<GuiPanel>();
			image->SetTexture(texture);
		}

		return imageObject;
	}

};