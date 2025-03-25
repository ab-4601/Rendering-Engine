#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <imgui.h>

#define YELLOW  ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define CYAN    ImVec4(0.0f, 1.0f, 1.0f, 1.0f)
#define RED     ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define GRAY    ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
#define MAROON  ImVec4(0.5f, 0.0f, 0.0f, 1.0f)
#define GOLD    ImVec4(1.0f, 0.84f, 0.0f, 1.0f)
#define GREEN   ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define MAGENTA ImVec4(1.0f, 0.0f, 1.0f, 1.0f)

struct LogFragment {
	std::string text{};
	ImVec4 color{ 1.f, 1.f, 1.f, 1.f };
};

struct DebugLogEntry {
	std::vector<LogFragment> fragments;
};

static const char* sFormatStart = "---------------------OpenGL-callback-start------------\n";
static const char* sMessageLabel = "Message: ";
static const char* sTypeLabel = "Type: ";
static const char* sIdLabel = "Id: ";
static const char* sSeverityLabel = "Severity: ";
static const char* sFormatEnd = "---------------------OpenGL-callback-end--------------\n";

static const char* eTypes[] = { "ERROR", "DEPRECATED_BEHAVIOR", "UNDEFINED_BEHAVIOR", "PORTABILITY", "PERFORMANCE", "OTHER" };
static const ImVec4 eTypesC[] = { RED, GRAY, MAROON, GOLD, GREEN, MAGENTA };

static const char* eSeverities[] = { "HIGH", "MEDIUM", "LOW" };
static const ImVec4 eSeveritiesC[] = { RED, GOLD, GREEN };

extern std::vector<DebugLogEntry> g_DebugLogEntries;

void AddDebugLogEntry(GLenum source, GLenum type, GLuint id, GLenum severity, const char* message);
void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void RegisterDebugCallback();

#endif