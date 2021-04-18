//I used the code in the page that is given in the project description and inspired of the graph implementation used in ford_fulkerson ps

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <queue>
#include <limits>
#include <list>
#include <iterator>

const int MAX_N = 10000;
int parent[MAX_N];
int size;

using namespace std;

struct Vertex {
public:
    int ID;
    bool isVisited;
    unordered_map<int, int> adjacencyMap;

    Vertex(int ID=0) {
        this->ID = ID;
        isVisited = false;
    }

    void addEdge(int ID, int capacity) {
        this->adjacencyMap[ID] = capacity;
    }

    void eraseEdge(int ID){
        this->adjacencyMap.erase(ID);
    }
};

int cycleHandling(vector<Vertex> &graph, list<int> cycle){ //finds total voltage of the cycle and reverses edges
    
    int volt = 0; //volt of cycle
    int u, v;
    cycle.push_back(*cycle.begin()); //adding the first element to the end to get the last edge
    list<int>::iterator itr = cycle.begin();
    u = *itr;
    itr++;

    for( ; itr!=cycle.end() ; itr++){  //find total voltage of the cycle
        volt += graph[u].adjacencyMap[*itr];
        u=*itr;
    }
    if(volt>0){ //if its a negative cycle ( positive in out case )
        int edgeVoltage = 0;

        list<int>::iterator itr2 = cycle.begin(); 
        u = *itr2; //taking first vertex
        itr2++;

        for( ; itr2!=cycle.end() ; itr2++){
            v=*itr2;  //second vertex
            edgeVoltage=graph[u].adjacencyMap[v];  
            graph[u].adjacencyMap.erase(v);  // deleting edge
            graph[v].addEdge(u,edgeVoltage*(-1)); //adding reverse
            u=*itr2;
        }
        return volt;  //returning positive volt
    }
    return 0;  //the case is which there is no positive voltage in the cycle
}

list<int> getCycle(int v){  //geting the cycle as a list

    list<int> cycle;
    stack<int> stack;
    vector<bool> in_stack(size);

    while(!in_stack[v]){  
        stack.push(v);
        in_stack[v]=true;
        v=parent[v];
    }

    while(stack.top()!=v){  //getting cycle part
        cycle.push_back(stack.top());
        stack.pop();
    }

    cycle.push_back(v);
    return cycle;
}

int detect_cycle(){ //by using parent[], detects whether there is a cycle exists or not

    bool on_stack[size];
    bool visited[size];
    fill(on_stack, on_stack + size, false);  
    fill(visited, visited + size, false); 
    vector<int> vec;
    
    for (int i = 1; i < size ; ++i){  //if the vertex i not visited
                if (!visited[i]){
                            for (int j = i; j != -1; j = parent[j]) { // take parent j, until no parent -1
                                        if (!visited[j]) {  //if j is not visited
                                            visited[j] = true; //make it visited
                                            on_stack[j] = true;  //make on_stack true
                                            vec.push_back(j);  //put j to the vector
                                        } else {  //or
                                            if (on_stack[j])
                                            return i; //there is a cycle
                                        }
                            }
                            for (int j : vec){
                                on_stack[j] = false;
                            }
                            vec.clear();
                }
    }
    return -1;  //there is no cycle
}

int spfa_early_terminate(vector<Vertex> &graph){

    int volt[size];
    bool in_queue[size];

    fill(volt, volt + graph.size(), 0);  //volt
    fill(parent, parent + graph.size(), -1);  //parent
    fill(in_queue, in_queue + graph.size(), true);  //in_queue

    queue<int> queue;
    for (int i = 1; i < graph.size(); ++i){
        queue.push(i);
    }

    int iter = 0;  //for counting
    int j, u, v;

    while (!queue.empty()){
        u = queue.front();  //ilk vertex 0'ı aldık
        queue.pop();
        in_queue[u] = false; //u queue içinde değil artık
        for (auto edge : graph[u].adjacencyMap){ //for (u,v) in E(G)   0'ın adjacency list
            v = edge.first;
            if (volt[u] + edge.second > volt[v]){
                parent[v] = u;  // v'nin parent'ı u
                volt[v] = volt[u] + edge.second; //cost update
                if (++iter == graph.size()-1){
                    iter = 0;
                    j=detect_cycle();
                    if(j!=-1){
                        list<int> cycle = getCycle(j);
                        return cycleHandling(graph,cycle);;
                    } 
                }
                if (!in_queue[v]){
                    queue.push(v);
                    in_queue[v] = true;
                }
            }
        }
    }

    j=detect_cycle();
     if(j=!-1){
       list<int> cycle = getCycle(j);
       return cycleHandling(graph,cycle);
   } 
    return -1; //loop end
}


int makePath(vector<Vertex> &graph){ //makes arbitrary initial edges, removes used edges, adds reverse edges 

    int voltage = 0;
    for(int i=1 ; i<graph.size()/2+1 ; i++) { 
       
        int vertexFromId = i;
        for (auto edge : graph[vertexFromId].adjacencyMap){
            int vertexToId = edge.first;  //name of other vertex

            if(!graph[vertexToId].isVisited){
                graph[vertexToId].isVisited = true;
                int edgeVoltage = edge.second;  //cost of the edge
                voltage += edgeVoltage;   // adding cost to voltage
                graph[vertexFromId].eraseEdge(vertexToId);
                graph[vertexToId].addEdge(vertexFromId,edgeVoltage*(-1)); //adding reverse
                break;
            }
        }
    }
    return voltage;
}

int main(int argc, char const *argv[]) {
    
    string infile_name = argv[1];  //file input
    string outfile_name = argv[2]; //file output
   
    ifstream infile;
    infile.open(infile_name);

    ofstream outfile;
    outfile.open(outfile_name);

    int numbOfTestCases, dimOfMatrix, voltage;

    infile >> numbOfTestCases;
    int vertex_id_1, vertex_id_2; //names of the vertices, from 1 to 2*dimOfMatrix

    for (int i = 0; i < numbOfTestCases; ++i){
        
        infile >> dimOfMatrix;
        size = 2*dimOfMatrix+1;
        vector<Vertex> graph(size);
      
        for (int j=1; j<dimOfMatrix+1 ; ++j){
            vertex_id_1 = j;
            graph[vertex_id_1] = Vertex(vertex_id_1);
            for (int k = dimOfMatrix+1; k < size; ++k) {
                infile >> voltage;
                vertex_id_2 = k;
                graph[vertex_id_2] = Vertex(vertex_id_2);
                graph[vertex_id_1].addEdge(vertex_id_2, voltage);
            }
        }
            int voltage = makePath(graph);  //getting initial arbitrary voltage
                    
            while(true) {  //while there exist a cycle, keeps deleting cycles and add the additional voltage to initial voltage
                int addVoltage = spfa_early_terminate(graph);  
                if(addVoltage!=-1){
                    voltage += addVoltage;
                } else {
                    break;
                }
            }
            outfile << voltage << endl;     
    }

    infile.close();
    outfile.close();
 
    return 0; 

} 
