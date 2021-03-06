/*
 MIT License

 Copyright (c) 2017 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <chrono>
#include <sstream>

#include <imgui.h>

#include <engine/engine.h>
#include <engine/log.h>
#include <utilities/file_utility.h>

#include <engine/window.h>
#include <graphics/graphics.h>
#include <utilities/file_utility.h>

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko
{
BasicEngine* BasicEngine::instance_ = nullptr;

BasicEngine::BasicEngine(Configuration* config)
{
    if (config != nullptr)
    {
        this->config = *config;
    }

#ifdef EASY_PROFILE_USE
    EASY_PROFILER_ENABLE;
#endif
}

BasicEngine::~BasicEngine()
{
#ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Neko_Profile.prof");
#endif
}
#ifdef __ANDROID__

#include <sys/stat.h>

extern "C"
JNIEXPORT void JNICALL
Java_swiss_sae_gpr5300_MainActivity_finalize([[maybe_unused]]JNIEnv *env, [[maybe_unused]]jclass clazz, [[maybe_unused]]jstring directory)
{

#ifdef EASY_PROFILE_USE
    if(env == nullptr)
    {
        logDebug("[Error] Android environment is null");
        return;
    }

    std::string path = env->GetStringUTFChars(directory, nullptr);

	path += "/Neko_Profile.prof";
	logDebug("Android data profile data path: "+path);
    auto blockNumber = profiler::dumpBlocksToFile(path.c_str());
    if(blockNumber == 0)
    {
        logDebug("[Error] Could not save profile data");
    }
    else
    {
        logDebug("Easy Profile with several blocks: "+std::to_string(blockNumber));
    }
#endif
}
#endif

void BasicEngine::Init()
{
    logManager_ = new LogManager;
	
#ifdef EASY_PROFILE_USE
    EASY_FUNCTION(profiler::colors::Magenta);
#endif
	instance_ = this;
	LogDebug("Current path: " + GetCurrentPath());
	jobSystem_.Init();
}

void BasicEngine::Update(seconds dt)
{
    dt_ = dt.count();
	Time::time += dt_;
	fixedUpdateAccumulator_ += dt_;
	Time::deltaTime = dt_;
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Basic Engine Update");
#endif
    renderer_->ResetJobs();
    window_->ResetJobs();
	
    Job eventJob([this]
    {
#if defined(__ANDROID__) or defined(EMSCRIPTEN)
        window_->MakeCurrentContext();
#endif
	    ManageEvent();
#if defined(__ANDROID__) or defined(EMSCRIPTEN)
        window_->LeaveCurrentContext();
#endif
    });
    Job updateJob([this, &dt]{updateAction_.Execute(dt);});
    updateJob.AddDependency(&eventJob);

    Job* rendererSyncJob = renderer_->GetSyncJob();
    updateJob.AddDependency(rendererSyncJob);

    Job* renderJob = renderer_->GetRenderAllJob();
    renderJob->AddDependency(&eventJob);

    Job* swapBufferJob = window_->GetSwapBufferJob();
    swapBufferJob->AddDependency(renderJob);
    swapBufferJob->AddDependency(&updateJob);

    renderer_->ScheduleJobs();
    jobSystem_.ScheduleJob(swapBufferJob, JobThreadType::RENDER_THREAD);
    jobSystem_.ScheduleJob(&eventJob, JobThreadType::MAIN_THREAD);
    jobSystem_.ScheduleJob(&updateJob, JobThreadType::MAIN_THREAD);
    while (fixedUpdateAccumulator_ > Time::fixedDeltaTime)
    {
	    fixedUpdateAccumulator_ -= Time::fixedDeltaTime;
	
        Job fixedUpdateJob([this]{fixedUpdateAction_.Execute();});
		jobSystem_.ScheduleJob(&fixedUpdateJob, JobThreadType::MAIN_THREAD);
    }
#ifndef NEKO_SAMETHREAD
    swapBufferJob->Join();
#else
    jobSystem_.KickJobs();
#endif
}

void BasicEngine::Destroy()
{
#ifndef NEKO_SAMETHREAD
    Job leaveContext([this] {window_->LeaveCurrentContext(); });
    jobSystem_.ScheduleJob(&leaveContext, JobThreadType::RENDER_THREAD);
#endif
    window_->MakeCurrentContext();
    renderer_->Destroy();
	jobSystem_.Destroy();
	instance_ = nullptr;
	
    logManager_->WriteToFile();
    LogDebug("Destroy Basic Engine");
    logManager_->Wait();
    logManager_->Destroy();
}

static std::chrono::time_point<std::chrono::system_clock> clock;
#ifdef EMSCRIPTEN
void EmLoop(void* arg)
{
    BasicEngine* engine = static_cast<BasicEngine*>(arg);
    const auto start = std::chrono::system_clock::now();
    const auto dt = std::chrono::duration_cast<seconds>(start - clock);
    clock = start;
    (engine)->Update(dt);
}
#endif

void BasicEngine::EngineLoop()
{
	isRunning_ = true;
    window_->LeaveCurrentContext();
	Job initRenderJob([this]{window_->MakeCurrentContext();});
	jobSystem_.ScheduleJob(&initRenderJob, JobThreadType::RENDER_THREAD);
	clock = std::chrono::system_clock::now();
#ifdef EMSCRIPTEN
    // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);

    emscripten_set_main_loop_arg(&EmLoop, this, 0, 1);
#else
	while (isRunning_)
	{
		const auto start = std::chrono::system_clock::now();
		const auto dt = std::chrono::duration_cast<seconds>(start - clock);
		clock = start;
		Update(dt);
	}
#endif
    Destroy();
}

void BasicEngine::SetWindowAndRenderer(Window* window, Renderer* renderer)
{
    window_ = window;
    renderer_ = renderer;
    renderer_->SetWindow(window);
    RendererLocator::provide(renderer);
}

void BasicEngine::GenerateUiFrame()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Generate ImGui Frame");
#endif
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::Begin("Neko Window");

	std::ostringstream oss;
	oss << "App FPS: " << 1.0f / dt_ << '\n'
		<< '\n';
	ImGui::Text("%s", oss.str().c_str());
	ImGui::End();
	drawImGuiAction_.Execute();
}

void BasicEngine::RegisterSystem(SystemInterface& system)
{
    initAction_.RegisterCallback([&system]{system.Init();});
    updateAction_.RegisterCallback([&system](const seconds dt){system.Update(dt);});
    fixedUpdateAction_.RegisterCallback([&system]{system.FixedUpdate();});
    destroyAction_.RegisterCallback([&system]{system.Destroy();});
}

void BasicEngine::RegisterOnDrawUi(DrawImGuiInterface& drawUi)
{
    drawImGuiAction_.RegisterCallback([&drawUi]{ drawUi.DrawImGui();});
}

void BasicEngine::ScheduleJob(Job* job, JobThreadType threadType)
{
    jobSystem_.ScheduleJob(job, threadType);
}


}
