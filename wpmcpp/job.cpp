#include <math.h>

#include "qdebug.h"
#include "qmutex.h"

#include "job.h"

JobState::JobState()
{
    this->cancelRequested = false;
    this->completed = false;
    this->errorMessage = "";
    this->hint = "";
    this->progress = 0;
}

JobState::JobState(const JobState& s)
{
    this->cancelRequested = s.cancelRequested;
    this->completed = s.completed;
    this->errorMessage = s.errorMessage;
    this->hint = s.hint;
    this->progress = s.progress;
}

QList<Job*> Job::jobs;

Job::Job()
{
    this->progress = 0.0;
    this->parentJob = 0;
    this->cancelRequested = false;
    this->completed = false;
}

Job::~Job()
{
    if (parentJob) {
        parentJob->setHint(parentHintStart);
    }
}

void Job::complete()
{
    if (!this->completed) {
        this->completed = true;
        fireChange();
    }
}

bool Job::isCancelled()
{
    return this->cancelRequested;
}

void Job::cancel()
{
    // qDebug() << "Job::cancel";
    if (!this->cancelRequested) {
        this->cancelRequested = true;
        // qDebug() << "Job::cancel.2";
        if (parentJob)
            parentJob->cancel();

        fireChange();
        // qDebug() << "Job::cancel.3";
    }
}

void Job::parentJobChanged(const JobState& s)
{
    // qDebug() << "Job::parentJobChanged";
    if (s.cancelRequested && !this->isCancelled())
        this->cancel();
}

Job* Job::newSubJob(double nsteps)
{
    Job* r = new Job();
    r->parentJob = this;
    r->subJobSteps = nsteps;
    r->subJobStart = this->getProgress();
    r->parentHintStart = hint;

    // bool success =
    connect(this, SIGNAL(changed(const JobState&)),
            r, SLOT(parentJobChanged(const JobState&)),
            Qt::DirectConnection);
    // qDebug() << "Job::newSubJob " << success;
    return r;
}

bool Job::isCompleted()
{
    return this->completed;
}

void Job::fireChange()
{
    JobState state;
    state.cancelRequested = this->cancelRequested;
    state.completed = this->completed;
    state.errorMessage = this->errorMessage;
    state.hint = this->hint;
    state.progress = this->progress;
    emit changed(state);
}

void Job::setProgress(double progress)
{
    this->progress = progress;

    fireChange();

    updateParentProgress();
    updateParentHint();
}

void Job::updateParentProgress()
{
    if (parentJob) {
        double d = this->subJobStart +
                this->getProgress() * this->subJobSteps;
        parentJob->setProgress(d);
    }
}

double Job::getProgress() const
{
    return this->progress;
}

QString Job::getHint() const
{
    return this->hint;
}

void Job::setHint(const QString &hint)
{
    this->hint = hint;

    fireChange();

    updateParentHint();
}

void Job::updateParentHint()
{
    if (parentJob) {
        if ((isCompleted() && getErrorMessage().isEmpty())
                || this->hint.isEmpty())
            parentJob->setHint(parentHintStart);
        else
            parentJob->setHint(parentHintStart + " / " + this->hint);
    }
}

QString Job::getErrorMessage() const
{
    return this->errorMessage;
}

void Job::setErrorMessage(const QString &errorMessage)
{
    this->errorMessage = errorMessage;

    fireChange();
}
