#include "../include/main.hpp"
#include "../include/builtins.hpp"

std::vector<int (*)(std::vector<std::string> args)> builtins = {&test};
std::vector<std::string> builtins_str = {"test"};
bool piped = false;

int main(int argc, char* argv[])
{
    std::string cmd_str;
    std::vector<std::string> cmd_history;
    std::vector<std::string> cmd;
    std::vector<std::string> pipe_cmd;
    bool status;
    
    do {

        // get the input of the user and place it in cmd_str
        std::cout << "~$ ";
        getline(std::cin, cmd_str);

        // process and execute the command if the input is not empty
        if(!cmd_str.empty()){

            status = true;

            cmd_history.push_back(cmd_str);
            cmd.clear();
            pipe_cmd.clear();
            tokenize(cmd, pipe_cmd, cmd_str);
            if(cmd[0] == "exit"){ std::exit(EXIT_SUCCESS); }
            execute_builtin(cmd, pipe_cmd);
            piped = false;

        } else { std::exit(EXIT_FAILURE); }

    } while (status);

    std::exit(EXIT_SUCCESS);
}

/** function that takes in a vector and a string
 *  extracts every string seperated by a space and pushes it back to the vector */
void tokenize(std::vector<std::string> &args, std::vector<std::string> &piped_args, std::string str){

    std::string temp;
    int pos = 1;

    // iterate through each char in str
    for(std::string::iterator it = str.begin(); it != str.end(); ++it){

        // let c be the char
        std::string c(1, *it);

        if(c == "|") { piped = true; }

        // add the char to the string temp if it is not a space
        if(c != " " && c != "|"){
            temp.append(c);
        } else {
            // add the string temp to the vector of strings vect and reset the temp string
            if(!temp.empty()){
                if(!piped){ args.push_back(temp); } else { piped_args.push_back(temp); }
                temp = "";
            }
        }

        // add the last string to the vector if it is not empty
        if(pos == str.length() && !temp.empty() && !(temp == "|")){
            if(!piped){ args.push_back(temp); } else { piped_args.push_back(temp); }
        }
        
        pos++;
    }

}

/** function that takes in a vector of strings
 *  checks if the command entered by the user matches a builtin command and executes it through the shell */
int execute_builtin(std::vector<std::string> &args, std::vector<std::string> &piped_args){

    // iterate through the vector of inputted strings and compare the command typed to built in functions to find which built in function to execute
    if(!args[0].empty()) {
        for(int i = 0; i < builtins.size(); i++){
            if(args[0] == builtins_str[i]){
                return (*builtins[i])(args);
            }
        }
    } else { std::exit(EXIT_FAILURE); }

    if(!piped){
        return execute_cmd(args);
    } else{
        return execute_pipe_cmd(args, piped_args);
    }
}

/** function that takes in a vector of strings
 *  it creates a child process and runs the command inputted by the user with arguments if any */
int execute_cmd(std::vector<std::string> str_args){

    // convert every std string in the vector str_args to char* strings and place them in a char* array
    char* args[str_args.size()+1];
    int i;
    for(i = 0; i < str_args.size(); i++){
        args[i] = (char*)str_args[i].c_str();
    }
    args[i] = NULL;
    
    pid_t pid, wpid;
    int status;

    // fork a child process
    pid = fork();

    if(pid == -1) { 
        perror("forking");
        return EXIT_FAILURE;
    }
    // inside child process
    if(pid == 0){
    if(execvp(args[0], args) == -1){
        perror("exec");
        std::exit(EXIT_FAILURE);
    }
    std::exit(EXIT_FAILURE);
    } else{

        // wait for child to change state until the child process terminates
        do{
            wpid = waitpid(pid, &status, 0);
        } while(!WIFEXITED(status));
    }

    return EXIT_SUCCESS;
}

int execute_pipe_cmd(std::vector<std::string> str_args, std::vector<std::string> str_piped_args){

    char* args[str_args.size() + 1];
    char* piped_args[str_piped_args.size() + 1];
    int i, j;

    for(i = 0; i < str_args.size(); i++){
        args[i] = (char*)str_args[i].c_str();
    }
    for(j = 0; j < str_piped_args.size(); j++){
        piped_args[j] = (char*)str_piped_args[j].c_str();
    }    
    args[i] = NULL;
    piped_args[j] = NULL;

    // fd[0] = read end and fd[1] = write end
    int fd[2];
    int status;
    pid_t pid1, pid2, wpid;

    if(pipe(fd) == -1){
        perror("piping");
    }

    pid1 = fork();
    if(pid1 == -1) { 
        perror("forking first child");
        return EXIT_FAILURE;
    }
    // inside the child process
    if(pid1 == 0){

        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        if(execvp(args[0], args) == -1){
            perror("executing first command");
            std::exit(EXIT_FAILURE);
        }
        std::exit(EXIT_FAILURE);
    } else{

        //  inside the parent process
        pid2 = fork();

        if(pid2 == -1){
            perror("forking second child");
            return EXIT_FAILURE;
        } 
            
        if(pid2 == 0){

            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);

            if(execvp(piped_args[0], piped_args) == -1){
                perror("executing second command");
                std::exit(EXIT_FAILURE);
        }
        std::exit(EXIT_FAILURE);
        } else {
            close(fd[1]);
            close(fd[0]);
            do{
                wpid = waitpid(pid1, &status, 0);
                wpid = waitpid(pid2, &status, 0);
            } while(!WIFEXITED(status));
        }
    }
    return EXIT_SUCCESS;

}
