#include <engine/jobsystem.h>

#include <utility>

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko
{

JobSystem::JobSystem()
{

}

JobSystem::~JobSystem()
{

}

void JobSystem::ScheduleJob(Job* func, JobThreadType threadType)
{
    auto scheduleJobFunc = [&func](JobQueue& jobQueue)
    {
#ifndef NEKO_SAMETHREAD
        std::unique_lock<std::mutex> lock(jobQueue.mutex_);
        jobQueue.jobs_.push(func);
        jobQueue.cv_.notify_one();
#else
        jobQueue.jobs_.push(func);
#endif

    };
	switch (threadType)
	{
    case JobThreadType::MAIN_THREAD:
    {
        func->Execute();
        break;
    }
    case JobThreadType::RENDER_THREAD:
	{
        scheduleJobFunc(renderJobs_);
        break;
    }
    case JobThreadType::RESOURCE_THREAD:
	{
        scheduleJobFunc(resourceJobs_);
    	break;
    }
    case JobThreadType::OTHER_THREAD:
	{
        scheduleJobFunc(jobs_);
        break;
    }
	default: ;
	}
}
#ifdef NEKO_SAMETHREAD
void JobSystem::KickJobs()
{
    Work(resourceJobs_);
    Work(jobs_);
    Work(renderJobs_);
}
#endif
void JobSystem::Work(JobQueue& jobQueue)
{
    {
#ifndef NEKO_SAMETHREAD
        std::lock_guard<std::mutex> lock(statusMutex_);
        ++workersStarted_;
#endif
    }
    while (IsRunning())
    {
        Job* job = nullptr;
        {// CRITICAL
#ifndef NEKO_SAMETHREAD
            std::unique_lock<std::mutex> lock(jobQueue.mutex_);
#endif
            if (!jobQueue.jobs_.empty())
            {
                job = jobQueue.jobs_.front();
                jobQueue.jobs_.pop();
                if (!job->CheckDependenciesStarted())
                {
#ifndef NEKO_SAMETHREAD
                	if(jobQueue.jobs_.empty())
                	{
#ifdef EASY_PROFILE_USE
                        EASY_BLOCK("Wait for Dependencies");
#endif

                        jobQueue.cv_.wait_for(lock, std::chrono::microseconds(100));

                	}
#endif
                    jobQueue.jobs_.push(job);
                    continue;
                }
            }
            else
            {
#ifdef NEKO_SAMETHREAD
                break;
#else
                if (IsRunning()) // Atomic check.
                {
                    jobQueue.cv_.wait(lock); // !CRITICAL
                }
            	continue; //Refetch a new job
#endif
            }
        }// !CRITICAL
        job->Execute();
    }
}

void JobSystem::Init()
{
    status_ = RUNNING;
#ifndef NEKO_SAMETHREAD
    numberOfWorkers = std::max(3u, std::thread::hardware_concurrency() - 1);
    workers_.resize(numberOfWorkers);

    const size_t len = numberOfWorkers;
    for (size_t i = 0; i < len; ++i)
    {
        switch (i)
        {
            case static_cast<int>(JobThreadType::RENDER_THREAD):
            {
                workers_[i] = std::thread([this] { Work(renderJobs_); }); // Kick the thread => sys call
                break;
            }
            case static_cast<int>(JobThreadType::RESOURCE_THREAD):
            {
                workers_[i] = std::thread([this] { Work(resourceJobs_); }); // Kick the thread => sys call
                break;
            }
            default:
            {
                workers_[i] = std::thread([this] { Work(jobs_); }); // Kick the thread => sys call
                break;
            }
        }

    }
#endif
}

void JobSystem::Destroy()
{
#ifndef NEKO_SAMETHREAD
// Spin-lock waiting for all threads to become ready for shutdown.
    std::function<bool()> checkFunc = [this]()
    {
        std::lock_guard<std::mutex> lock(statusMutex_);
        return workersStarted_ != numberOfWorkers ||
                  !jobs_.jobs_.empty() ||
                  !renderJobs_.jobs_.empty() ||
                  !resourceJobs_.jobs_.empty();
    };
    while (checkFunc())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (1));
    }
#endif
    status_ = NONE;
#ifndef NEKO_SAMETHREAD
    renderJobs_.cv_.notify_all();
    resourceJobs_.cv_.notify_all();
	jobs_.cv_.notify_all(); // Wake all workers.
    const size_t len = numberOfWorkers;
    for (size_t i = 0; i < len; ++i)
    {
        workers_[i].join(); // Join all workers.
    }
#endif
}

bool JobSystem::IsRunning()
{
#ifndef NEKO_SAMETHREAD
    std::lock_guard<std::mutex> lock(statusMutex_);
#endif
    return status_ & Status::RUNNING;
}
#ifndef NEKO_SAMETHREAD
std::uint8_t JobSystem::CountStartedWorkers()
{
    std::lock_guard<std::mutex> lock(statusMutex_);
    return workersStarted_;
}
#endif

Job::Job(std::function<void()> task) :
        task_(std::move(task)),
        status_(0)
{

}

Job::Job(Job&& job) noexcept
{
    dependencies_ = std::move(job.dependencies_);
    task_ = std::move(job.task_);
    status_ = job.status_;
}

Job& Job::operator=(Job&& job) noexcept
{
    dependencies_ = std::move(job.dependencies_);
    task_ = std::move(job.task_);
    status_ = job.status_;
    return *this;
}
#ifndef NEKO_SAMETHREAD
void Job::Join() const
{
    if(IsDone())
    {
        return;
    }

    for(const auto& dep : dependencies_)
    {
        dep->Join();
    }

    std::unique_lock<std::mutex> lock(executionLock_);
    if(!HasStarted())
    {
        cv_.wait(lock);
    }
}
#endif

void Job::Execute()
{
#ifndef NEKO_SAMETHREAD
    for (auto& dep : dependencies_)
    {
        if (!dep->IsDone())
        {
            dep->Join();
        }
    }
#endif

    {
#ifndef NEKO_SAMETHREAD
        std::lock_guard<std::mutex> lock(statusLock_);
#endif
        status_ |= STARTED;
    }
    {
#ifndef NEKO_SAMETHREAD
        std::lock_guard<std::mutex> lock(executionLock_);
        cv_.notify_all(); //Notify all the dependees that we started!
#endif
        task_();
    }
    {
#ifndef NEKO_SAMETHREAD
        std::lock_guard<std::mutex> lock(statusLock_);
#endif
        status_ |= DONE;
    }
}

bool Job::CheckDependenciesStarted() const
{
	for(auto& dep : dependencies_)
	{
		if(!dep->HasStarted())
            return false;
	}
    return true;
}

bool Job::HasStarted() const
{
#ifndef NEKO_SAMETHREAD
    std::lock_guard<std::mutex> lock(statusLock_);
#endif
    return status_ & STARTED;
}

bool Job::IsDone() const
{
#ifndef NEKO_SAMETHREAD
    std::lock_guard<std::mutex> lock(statusLock_);
#endif
    return status_ & DONE;
}

void Job::AddDependency(const Job* dependentJob)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Jobsystem Add Dependency");
#endif
    //Be sure to not create a cycle of dependencies which would deadlock the thread
	//Also check if the dependencies is not already in the dependency tree
    std::function<bool(const std::vector<const Job*>&, const Job*)> checkDependencies =
            [&checkDependencies, dependentJob](const std::vector<const Job*>& dependencies, const Job* job) {
                for(auto& dep : dependencies)
                {
                    if(dep == job || dep == dependentJob)
                        return false;
                    const bool recursiveDep = checkDependencies(dep->dependencies_, job);
                    if(!recursiveDep)
                        return false;
                }
                return true;
            };
    if(checkDependencies(dependencies_, this))
    {
        dependencies_.push_back(dependentJob);
    }
}

void Job::Reset()
{
    status_ = 0;
    dependencies_.clear();
}




}
