#include <iostream>
#include <fstream>
#include <ranges>
#include <vector>
#include <string>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <fcntl.h>

using namespace std;

vector<string> service_files;
volatile sig_atomic_t shutdown_flag = 0;

struct Service {
    int number;
    string path;

    Service(const int n, string  p) : number(n), path(std::move(p)) {}

    bool operator<(const Service& other) const {
        return number < other.number;
    }
};

void signal_handler(const int sig) {
    if(sig == SIGTERM || sig == SIGINT) {
        shutdown_flag = 1;
    }
}

void sigchld_handler(int sig) {
    while(waitpid(-1, nullptr, WNOHANG) > 0);
}

vector<Service> get_services(const string& init_dir) {
    vector<Service> services;

    if(DIR *dir; (dir = opendir(init_dir.c_str())) != nullptr) {
        dirent *ent;
        while((ent = readdir(dir)) != nullptr) {
            string name = ent->d_name;
            if(name == "." || name == "..") continue;

            try {
                const size_t dash = name.find('-');
                if(dash == string::npos) continue;

                int num = stoi(name.substr(0, dash));
                services.emplace_back(num, init_dir + "/" + name);
            }
            catch(...) {
                cerr << "invalid service file: " << name << endl;
            }
        }
        closedir(dir);
    }
    else {
        perror("opendir");
    }

    sort(services.begin(), services.end());
    return services;
}

bool execute_service(const string& path, const string& action) {
    const pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return false;
    }

    if(pid == 0) { // Child process
        execl("/bin/sh", "sh", path.c_str(), action.c_str(), static_cast<char *>(nullptr));
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else { // Parent process
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

void start_services(const vector<Service>& services) {
    cout << "dendro is booting the system." << endl;
    for(const auto& service : services) {
        cout << "starting " << service.path << endl;
        if(!execute_service(service.path, "start")) {
            cerr << "failed to start service: " << service.path << endl;
        }
    }
}

void stop_services(const vector<Service>& services) {
    cout << "dendro is shutting down the system..." << endl;
    for(const auto & service : std::ranges::reverse_view(services)) {
        cout << "stopping " << service.path << endl;
        if(!execute_service(service.path, "stop")) {
            cerr << "failed to stop service: " << service.path << endl;
        }
    }
    cout << "all services stopped. exiting." << endl;
}

void setup_signals() {
    struct sigaction sa{};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);

    // Handle SIGCHLD to prevent zombies
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, nullptr);
}

int main() {
    // Redirect standard I/O to /dev/console
    const int console = open("/dev/console", O_WRONLY);
    dup2(console, STDOUT_FILENO);
    dup2(console, STDERR_FILENO);
    close(console);

    setup_signals();
    const auto services = get_services("/etc/dendro/init.d");
    start_services(services);

    while(!shutdown_flag) {
        pause(); // Wait for signals
    }

    stop_services(services);
    return 0;
}