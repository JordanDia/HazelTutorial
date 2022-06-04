#include <Hazel.h>

#include "imgui/imgui.h"

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		// Rendering a triangle
		
		m_VertexArray = std::shared_ptr<Hazel::VertexArray>(Hazel::VertexArray::Create());
		//m_VertexArray.reset(VertexArray::Create());
		float vertices[3 * 7] = {
			-0.5f, -0.5f, +0.0f, +0.6f, +0.8f, +1.0f, +1.0f,
			+0.5f, -0.5f, +0.0f, +0.0f, +0.0f, +1.0f, +1.0f,
			+0.0f, +0.5f, +0.0f, +0.2f, +0.9f, +1.0f, +1.0f
		};

		std::shared_ptr<Hazel::VertexBuffer> vertexBuffer = std::shared_ptr<Hazel::VertexBuffer>(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));

		Hazel::BufferLayout layout = {
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float4, "a_Color" },
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		std::shared_ptr<Hazel::IndexBuffer> indexBuffer = std::shared_ptr<Hazel::IndexBuffer>(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA = std::shared_ptr<Hazel::VertexArray>(Hazel::VertexArray::Create());
		//m_SquareVA.reset(VertexArray::Create());
		float squareVertices[4 * 3] = {
			-0.75f, -0.75f, +0.0f,
			+0.75f, -0.75f, +0.0f,
			+0.75f, +0.75f, +0.0f,
			-0.75f, +0.75f, +0.0f
		};

		std::shared_ptr<Hazel::VertexBuffer> squareVB = std::shared_ptr<Hazel::VertexBuffer>(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Hazel::IndexBuffer> squareIB = std::shared_ptr<Hazel::IndexBuffer>(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSource = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			
			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSource = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = std::make_shared<Hazel::Shader>(vertexSource, fragmentSource);
		//m_Shader.reset(new Shader());

		std::string blueShaderVertexSource = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			
			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string blueShaderFragmentSource = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader = std::make_shared<Hazel::Shader>(blueShaderVertexSource, blueShaderFragmentSource);
		//m_Shader.reset(new Shader());
	}

	void OnUpdate() override
	{
		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraPosition.x -= m_CameraMoveSpeed;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraPosition.x += m_CameraMoveSpeed;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_W))
			m_CameraPosition.y += m_CameraMoveSpeed;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_S))
			m_CameraPosition.y -= m_CameraMoveSpeed;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			m_CameraRotation += m_CameraRotationSpeed;
		if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			m_CameraRotation -= m_CameraRotationSpeed;


		Hazel::RenderCommand::SetClearColor({ 0.15f, 0.15f, 0.15f, 1.0f });
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hazel::Renderer::BeginScene(m_Camera);

		Hazel::Renderer::Submit(m_SquareVA, m_BlueShader);
		Hazel::Renderer::Submit(m_VertexArray, m_Shader);

		Hazel::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(Hazel::Event& event) override
	{

	}

private:
	std::shared_ptr<Hazel::Shader> m_Shader;
	std::shared_ptr<Hazel::VertexArray> m_VertexArray;

	std::shared_ptr<Hazel::Shader> m_BlueShader;
	std::shared_ptr<Hazel::VertexArray> m_SquareVA;

	Hazel::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 2.0f;
};

class TriangleLayer : public Hazel::Layer
{
public:
	TriangleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate()
	{

	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent()
	{

	}
};


class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}
