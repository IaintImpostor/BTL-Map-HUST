#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#define M_PI 3.14159265358979323846
#define MAX_NODES 50
#define EARTH_RADIUS 6371.0 
#define INF 1e9  
 
// Cấu trúc node(vị trí)
typedef struct Node {
    int id;
    double latitude;//vĩ độ
    double longitude;//tungđộ
    char name[30];  
} Node;
 
// Cấu trúc đường đi(edge)
typedef struct Edge {
    int src, dest;//scr:điểm đầu,dest:điểm cuối
    float weight; // kilometer, khoảng cách hai node
} Edge;
 
// Cấu trúc đồ thị
typedef struct Graph {
    int numNodes;//số lượng node
    int numEdges;//số lượng cạnh
    Node nodes[MAX_NODES];//danh sách các node
    Edge edges[MAX_NODES * (MAX_NODES - 1) / 2]; // Số đường đi ko phân biệt phía
} Graph;
 
// Thuật toán haversine tính toán khoảng cách giữa 2 điểm khi biết tọa độ
double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
 
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
 
    double a = pow(sin(dLat / 2), 2) +
               pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
 
    return EARTH_RADIUS * c;
}
 
// hàm tạo graph
Graph* createGraph(int numNodes) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numNodes = numNodes;
    graph->numEdges = 0;
    return graph;
}
 
// Thêm node vào graph
void addNode(Graph* graph, int id, double latitude, double longitude, char* name) {
    graph->nodes[id].id = id;
    graph->nodes[id].latitude = latitude;
    graph->nodes[id].longitude = longitude;
    snprintf(graph->nodes[id].name, sizeof(graph->nodes[id].name), "%s", name);
}
 
// Tạo đường đi đã tính toán giữa 2 node(cả 2 chiều)
void addEdge(Graph* graph, int src, int dest) {
    double lat1 = graph->nodes[src].latitude;
    double lon1 = graph->nodes[src].longitude;
    double lat2 = graph->nodes[dest].latitude;
    double lon2 = graph->nodes[dest].longitude;
 
    double distance = haversine(lat1, lon1, lat2, lon2);
 
    
    graph->edges[graph->numEdges].src = src;
    graph->edges[graph->numEdges].dest = dest;
    graph->edges[graph->numEdges].weight = distance;
    graph->numEdges++;
 
    
    graph->edges[graph->numEdges].src = dest;
    graph->edges[graph->numEdges].dest = src;
    graph->edges[graph->numEdges].weight = distance;
    graph->numEdges++;
}
 
// Thuật toán tìm đường Dijkstra
void dijkstra(Graph* graph, int start, float dist[], int prev[]) {
    bool visited[MAX_NODES] = {false};
 
    
    for (int i = 0; i < graph->numNodes; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;
 
    for (int i = 0; i < graph->numNodes; i++) {
        int u = -1;
        for (int j = 0; j < graph->numNodes; j++) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }
 
        // Node đã đi qua
        visited[u] = true;
 
        // Relax edges
        for (int j = 0; j < graph->numEdges; j++) {
            Edge edge = graph->edges[j];
            if (edge.src == u && !visited[edge.dest] && dist[u] + edge.weight < dist[edge.dest]) {
                dist[edge.dest] = dist[u] + edge.weight;
                prev[edge.dest] = u;
            }
        }
    }
}
 
// Hàm in đường đi
void printPath(int prev[], int start, int end, Graph* graph) {
    if (end == -1) {
        printf("No path found.\n");
        return;
    }
 
    // Xây lại đường đi dùng backtracking
    int path[MAX_NODES];
    int pathLength = 0;
    int current = end;
 
    while (current != -1) {
        path[pathLength++] = current;
        current = prev[current];
    }
 
    
    printf("Đường đi ngắn nhất: ");
    for (int i = pathLength - 1; i >= 0; i--) {
        printf("%s", graph->nodes[path[i]].name);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
}
 
// Hàm thực hiện tìm đường đi ngắn nhất
void calculateShortestDistance(Graph* graph, int start, int end) {
    float dist[MAX_NODES];
    int prev[MAX_NODES];
 
    dijkstra(graph, start, dist, prev);
 
    if (dist[end] == INF) {
        printf("Không tìm thấy đường đi từ %s đến %s.\n", graph->nodes[start].name, graph->nodes[end].name);
    } else {
        printf("Khoảng cách ngắn nhất từ %s đến %s: %.2f km\n", graph->nodes[start].name, graph->nodes[end].name, dist[end]);
        printPath(prev, start, end, graph);
    }
}
void findNearestParking(double currentLat, double currentLon, Node parkingLots[], int numParkingLots) {
    int nearestIndex = -1;
    double minDistance = INF;

    for (int i = 0; i < numParkingLots; i++) {
        double distance = haversine(currentLat, currentLon, parkingLots[i].latitude, parkingLots[i].longitude);
        if (distance < minDistance) {
            minDistance = distance;
            nearestIndex = i;
        }
    }

    if (nearestIndex != -1) {
        printf("Nhà để xe gần nhất là: %s (%.6f, %.6f)\n", 
               parkingLots[nearestIndex].name, 
               parkingLots[nearestIndex].latitude, 
               parkingLots[nearestIndex].longitude);
        printf("Khoảng cách: %.2f km\n", minDistance);
    } else {
        printf("Không tìm thấy nhà để xe gần nhất.\n");
    }
}
 
int main() {
    //Tạo graph
    int numNodes = 26; 
    Graph* graph = createGraph(numNodes);
 
    
    addNode(graph, 0, 21.007062075822986, 105.84256538050096, "C1");
    addNode(graph, 1, 21.00646028212118, 105.8423548431973, "C2");
    addNode(graph, 2, 21.006629402018316, 105.84389218980324, "C3");
    addNode(graph, 3, 21.00619639182183, 105.84411619643684, "C4");
    addNode(graph, 4, 21.00581311319953, 105.84414968187284, "C5");
    addNode(graph, 5, 21.006201226536913, 105.84498921328576, "C6");
    addNode(graph, 6, 21.005314469858547, 105.8453203972225, "C7/nhà xe C7");
    
    addNode(graph, 8, 21.00581351307712, 105.84234508115631, "C9");
    addNode(graph, 9, 21.00545504644995, 105.84393242295812, "C10");
    addNode(graph, 10, 21.004456360428474, 105.84223242837827, "D2");
    addNode(graph, 11, 21.004801472483248, 105.84474285369039, "D3");
    addNode(graph, 12, 21.00421096989095, 105.84213586885853, "D4");
    addNode(graph, 13, 21.004455960549485, 105.84501335316558, "D5");
    addNode(graph, 17, 21.004382947603666, 105.84262773823755, "D6");
    addNode(graph, 18, 21.00399232554944, 105.84289730023009, "D8");
    addNode(graph, 19, 21.00381615417098, 105.84440891865603, "D9");
    addNode(graph, 20, 21.004140458345802, 105.8449302046926, "D7");
    addNode(graph, 14, 21.005192129743122, 105.84530303172086, "Cổng Trần Đại Nghĩa");
    addNode(graph, 15, 21.00511247459493, 105.84135961234064, "Cổng Parabol");
    addNode(graph, 16, 21.004107509081983, 105.84333584138216, "Hồ tiền");
    addNode(graph, 21, 21.004421376697916, 105.84411155875196, "Thư viện TQB");
    addNode(graph, 22, 21.003996375531514, 105.84420690851762, "Nhà xe D9");
    addNode(graph, 23, 21.00592815917098, 105.84437201582207, "Nhà xe D4-D6");
    addNode(graph, 24, 21.004784453889897, 105.845458027659, "Nhà xe D3-5");
    addNode(graph, 25, 21.00449370315662, 105.84655700662798, "B1");
 
    
    addEdge(graph, 15, 8); 
    addEdge(graph, 15, 10); 
    addEdge(graph, 15, 12); 
    addEdge(graph, 8, 1);
    addEdge(graph, 1, 0);
    addEdge(graph, 0, 2);
    addEdge(graph, 2, 3);
    addEdge(graph, 3, 4);
    addEdge(graph, 4, 9);
    addEdge(graph, 9, 21);
    addEdge(graph, 12, 17);
    addEdge(graph, 12, 18);
    addEdge(graph, 17, 16);
    addEdge(graph, 18, 16);
    addEdge(graph, 16, 19);
    addEdge(graph, 21, 19);
    addEdge(graph, 21, 20);
    addEdge(graph, 21, 11);
    addEdge(graph, 21, 13);
    addEdge(graph, 19, 20);
    addEdge(graph, 20, 13);
    addEdge(graph, 13, 11);
    addEdge(graph, 11, 14);
    addEdge(graph, 11, 6);
    addEdge(graph, 6, 14);
    
    addEdge(graph, 6, 5);
    addEdge(graph, 5, 2);
    addEdge(graph, 9, 6);
    addEdge(graph, 10, 12);
    addEdge(graph, 15, 24);
    addEdge(graph, 24, 6);
    addEdge(graph, 4, 23);
    addEdge(graph, 23, 3);
    addEdge(graph, 19, 22);
    addEdge(graph, 22, 21);
    addEdge(graph, 14, 25);
 
   
for (int i = 0; i < graph->numNodes; i++) {
    if(i==7) continue;
    printf("%d. %s\n", i , graph->nodes[i].name);  // 
}
 
 
   
    int start, end;
    printf("Chọn điểm đầu (0-%d): ", numNodes-1);
    scanf("%d", &start);
    printf("Chọn điểm đến (0-%d): ", numNodes-1);
    scanf("%d", &end);
 
    
    if (start < 0 || start > numNodes || end < 0 || end > numNodes) {
        printf("Vui lòng nhập lại 2 node từ 0 đến %d.\n", numNodes);
        free(graph);
        return 0;
    }
 
    
    calculateShortestDistance(graph, start, end);  
    printf("\n");
    Node parkingLots[] = {
        {0, 221.003996375531514, 105.84420690851762, "Nhà để xe D9"},
        {1, 21.00592815917098, 105.84437201582207, "Nhà để xe D4-D6"},
        {2, 21.004784453889897, 105.845458027659, "Nhà để xe D35"},
        {3, 21.005314469858547, 105.8453203972225, "Nhà để xe C7"}
    };
    int numParkingLots = sizeof(parkingLots) / sizeof(parkingLots[0]);

    // Nhập tọa độ hiện tại
    double currentLat, currentLon;
    printf("Nhập tọa độ hiện tại (latitude longitude): ");
    scanf("%lf %lf", &currentLat, &currentLon);

    // Tìm nhà để xe gần nhất
    findNearestParking(currentLat, currentLon, parkingLots, numParkingLots); 
 
    
    free(graph);
    return 0;
}