#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <iomanip>

using namespace std;

// Structure to hold PageRank results
struct PageRankResult {
    vector<double> rank;
    bool converged;
    int finalIter;
    double finalDiff;
};

// Read edges and node IDs from file
void readData(
    const string& filename,
    vector<pair<long long, long long>>& edges,
    vector<long long>& nodeIds)
{
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: cannot open " << filename << endl;
        exit(1);
    }

    unordered_map<long long, bool> nodeSet;
    long long from, to;
    while (fin >> from >> to) {
        edges.push_back({from, to});
        nodeSet[from] = true;
        nodeSet[to] = true;
    }
    fin.close();

    nodeIds.reserve(nodeSet.size());
    for (const auto& kv : nodeSet) {
        nodeIds.push_back(kv.first);
    }
    sort(nodeIds.begin(), nodeIds.end());
}

// Build incoming adjacency list and out-degree table
void buildGraph(
    const vector<pair<long long, long long>>& edges,
    const vector<long long>& nodeIds,
    vector<vector<int>>& incoming,
    vector<int>& outDegree)
{
    int N = (int)nodeIds.size();
    incoming.assign(N, vector<int>());
    outDegree.assign(N, 0);

    unordered_map<long long, int> idToIndex;
    idToIndex.reserve(N);
    for (int i = 0; i < N; i++) {
        idToIndex[nodeIds[i]] = i;
    }

    for (const auto& e : edges) {
        int u = idToIndex[e.first];
        int v = idToIndex[e.second];
        incoming[v].push_back(u);
        outDegree[u]++;
    }
}

// Iterative PageRank computation with damping, teleport, and dead-end handling
PageRankResult computePageRank(
    const vector<vector<int>>& incoming,
    const vector<int>& outDegree,
    int maxIter,
    double damping,
    double eps)
{
    int N = (int)incoming.size();
    double invN = 1.0 / N;
    double teleport = (1.0 - damping) / N;

    vector<double> rank(N, invN);
    vector<double> newRank(N, 0.0);

    PageRankResult result;
    result.converged = false;
    result.finalIter = 0;
    result.finalDiff = 0.0;

    for (int iter = 0; iter < maxIter; iter++) {
        double danglingSum = 0.0;
        for (int i = 0; i < N; i++) {
            if (outDegree[i] == 0) {
                danglingSum += rank[i];
            }
        }
        double danglingContrib = damping * danglingSum / N;

        for (int v = 0; v < N; v++) {
            newRank[v] = teleport + danglingContrib;
            const vector<int>& inNodes = incoming[v];
            for (int u : inNodes) {
                newRank[v] += damping * rank[u] / outDegree[u];
            }
        }

        double diff = 0.0;
        for (int i = 0; i < N; i++) {
            diff += fabs(newRank[i] - rank[i]);
        }

        swap(rank, newRank);

        result.finalIter = iter + 1;
        result.finalDiff = diff;

        if (diff < eps) {
            result.converged = true;
            break;
        }
    }

    result.rank = move(rank);
    return result;
}

// Output top 100 nodes by PageRank score to file
void writeTop100(
    const string& filename,
    const vector<long long>& nodeIds,
    const vector<double>& rank)
{
    struct Result {
        long long nodeId;
        double score;
    };

    int N = (int)nodeIds.size();
    vector<Result> results(N);
    for (int i = 0; i < N; i++) {
        results[i].nodeId = nodeIds[i];
        results[i].score = rank[i];
    }

    int topK = min(100, N);
    partial_sort(
        results.begin(),
        results.begin() + topK,
        results.end(),
        [](const Result& a, const Result& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.nodeId < b.nodeId;
        });

    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Error: cannot write " << filename << endl;
        exit(1);
    }

    fout << fixed << setprecision(12);

    for (int i = 0; i < topK; i++) {
        fout << results[i].nodeId << " " << results[i].score << "\n";
    }

    fout.close();
}

int main() {
    vector<pair<long long, long long>> edges;
    vector<long long> nodeIds;

    readData("Data.txt", edges, nodeIds);

    sort(edges.begin(), edges.end());
    edges.erase(unique(edges.begin(), edges.end()), edges.end());

    vector<vector<int>> incoming;
    vector<int> outDegree;

    buildGraph(edges, nodeIds, incoming, outDegree);

    PageRankResult result = computePageRank(
        incoming,
        outDegree,
        10000,
        0.85,
        1e-8
    );

    if (result.converged) {
        cerr << "Converged in " << result.finalIter
             << " iterations. Final diff = "
             << result.finalDiff << endl;
    } else {
        cerr << "Error: PageRank did not converge within "
             << 10000 << " iterations. Final diff = "
             << result.finalDiff << endl;
        cerr << "Res.txt will not be generated." << endl;
        return 1;
    }

    double total = 0.0;
    for (double x : result.rank) {
        total += x;
    }

    cerr << fixed << setprecision(12);
    cerr << "Total PageRank = " << total << endl;

    if (fabs(total - 1.0) > 1e-6) {
        cerr << "Error: PageRank sum is not close to 1.0. "
             << "Res.txt will not be generated." << endl;
        return 1;
    }

    writeTop100("Res.txt", nodeIds, result.rank);

    return 0;
}
