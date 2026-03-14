#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <queue>
using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
};

class Scheduler {
protected:
    vector<Process> processes;

public:
    void loadProcesses(const string& filename) {
        ifstream infile(filename);
        Process p;
        while (infile >> p.pid >> p.arrival_time >> p.burst_time >> p.priority) {
            p.remaining_time = p.burst_time;
            p.waiting_time = 0;
            p.turnaround_time = 0;
            processes.push_back(p);
        }
        infile.close();
    }

    void writeOutput(const string& filename, const string& gantt_chart) {
        ofstream outfile(filename);
        for (auto& p : processes) {
            outfile << "Process " << p.pid << ": Waiting Time = " << p.waiting_time
                    << ", Turnaround Time = " << p.turnaround_time << endl;
        }
        outfile << "\nGantt Chart:\n" << gantt_chart;
        outfile.close();
    }

    virtual void schedule(const string& filename) = 0;
};

class FCFS : public Scheduler {
public:
    void schedule(const string& filename) override {
        sort(processes.begin(), processes.end(), [](Process a, Process b) {
            return a.arrival_time < b.arrival_time;
        });

        int current_time = 0;
        string gantt = "";
        for (auto& p : processes) {
            if (current_time < p.arrival_time)
                current_time = p.arrival_time;

            p.waiting_time = current_time - p.arrival_time;
            current_time += p.burst_time;
            p.turnaround_time = p.waiting_time + p.burst_time;

            gantt += "| P" + to_string(p.pid) + " ";
        }
        gantt += "|";

        writeOutput(filename, gantt);
    }
};

class SJF : public Scheduler {
public:
    void schedule(const string& filename) override {
        vector<Process> completed;
        int current_time = 0;
        string gantt = "";

        while (!processes.empty()) {
            vector<Process> ready;
            for (auto& p : processes)
                if (p.arrival_time <= current_time)
                    ready.push_back(p);

            if (ready.empty()) {
                current_time++;
                continue;
            }

            auto shortest = min_element(ready.begin(), ready.end(), [](Process a, Process b) {
                return a.burst_time < b.burst_time;
            });

            Process p = *shortest;
            current_time += p.burst_time;
            p.waiting_time = current_time - p.arrival_time - p.burst_time;
            p.turnaround_time = p.waiting_time + p.burst_time;

            gantt += "| P" + to_string(p.pid) + " ";
            completed.push_back(p);

            processes.erase(remove_if(processes.begin(), processes.end(), [&](Process x) {
                return x.pid == p.pid;
            }), processes.end());
        }

        gantt += "|";
        processes = completed;
        writeOutput(filename, gantt);
    }
};

class Priority : public Scheduler {
public:
    void schedule(const string& filename) override {
        vector<Process> completed;
        int current_time = 0;
        string gantt = "";

        while (!processes.empty()) {
            vector<Process> ready;
            for (auto& p : processes)
                if (p.arrival_time <= current_time)
                    ready.push_back(p);

            if (ready.empty()) {
                current_time++;
                continue;
            }

            auto highest = min_element(ready.begin(), ready.end(), [](Process a, Process b) {
                return a.priority < b.priority;
            });

            Process p = *highest;
            current_time += p.burst_time;
            p.waiting_time = current_time - p.arrival_time - p.burst_time;
            p.turnaround_time = p.waiting_time + p.burst_time;

            gantt += "| P" + to_string(p.pid) + " ";
            completed.push_back(p);

            processes.erase(remove_if(processes.begin(), processes.end(), [&](Process x) {
                return x.pid == p.pid;
            }), processes.end());
        }

        gantt += "|";
        processes = completed;
        writeOutput(filename, gantt);
    }
};

class RoundRobin : public Scheduler {
    int time_quantum;

public:
    RoundRobin(int tq) : time_quantum(tq) {}

    void schedule(const string& filename) override {
        queue<Process> q;
        vector<Process> original = processes;
        int current_time = 0;
        string gantt = "";

        while (!processes.empty() || !q.empty()) {
            for (auto& p : processes)
                if (p.arrival_time <= current_time)
                    q.push(p);

            processes.erase(remove_if(processes.begin(), processes.end(), [&](Process x) {
                return x.arrival_time <= current_time;
            }), processes.end());

            if (q.empty()) {
                current_time++;
                continue;
            }

            Process p = q.front(); q.pop();
            gantt += "| P" + to_string(p.pid) + " ";

            int exec_time = min(p.remaining_time, time_quantum);
            current_time += exec_time;
            p.remaining_time -= exec_time;

            for (auto& x : processes)
                if (x.arrival_time <= current_time)
                    q.push(x);
            processes.erase(remove_if(processes.begin(), processes.end(), [&](Process x) {
                return x.arrival_time <= current_time;
            }), processes.end());

            if (p.remaining_time > 0)
                q.push(p);
            else {
                for (auto& orig : original)
                    if (orig.pid == p.pid) {
                        p.waiting_time = current_time - orig.arrival_time - orig.burst_time;
                        p.turnaround_time = p.waiting_time + orig.burst_time;
                        break;
                    }
                original[p.pid - 1] = p;
            }
        }

        gantt += "|";
        processes = original;
        writeOutput(filename, gantt);
    }
};

int main() {
    string algorithm;
    cout << "Enter scheduling algorithm (FCFS/SJF/Priority/RR): ";
    cin >> algorithm;

    Scheduler* scheduler;

    if (algorithm == "FCFS") scheduler = new FCFS();
    else if (algorithm == "SJF") scheduler = new SJF();
    else if (algorithm == "Priority") scheduler = new Priority();
    else if (algorithm == "RR") {
        int tq;
        cout << "Enter time quantum: ";
        cin >> tq;
        scheduler = new RoundRobin(tq);
    } else {
        cout << "Invalid algorithm!\n";
        return 1;
    }

    scheduler->loadProcesses("input.txt");
    scheduler->schedule("output.txt");

    delete scheduler;
    cout << "Scheduling complete. Check output.txt\n";
    return 0;
}
