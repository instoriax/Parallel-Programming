#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>
#include <vector>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
void pageRank(Graph g, double *solution, double damping, double convergence)
{
  // initialize vertex weights to uniform probability. Double
  // precision scores are used to avoid underflow for large graphs

  int numNodes = num_nodes(g);
  double equal_prob = 1.0 / numNodes;
  /*
     For PP students: Implement the page rank algorithm here.  You
     are expected to parallelize the algorithm using openMP.  Your
     solution may need to allocate (and free) temporary arrays.

     Basic page rank pseudocode is provided below to get you started:
  */
     // initialization: see example code above
     //score_old[vi] = 1/numNodes;
     std::vector<double> score_old(numNodes,equal_prob);
     bool converged=0;


     while (!converged) {
      std::vector<double> score_new(numNodes,0);
       // compute score_new[vi] for all nodes vi:
       //score_new[vi] = sum over all nodes vj reachable from incoming edges
      //                    { score_old[vj] / number of edges leaving vj  }
      for (int j = 0; j < numNodes; ++j){
        double num=score_old[j]/outgoing_size(g,j);
        const Vertex* start = outgoing_begin(g, j);
        const Vertex* end = outgoing_end(g, j);
        for (const Vertex* v=start; v!=end; v++){
          score_new[*v]+=num;
        }
      }
       //score_new[vi] = (damping * score_new[vi]) + (1.0-damping) / numNodes;
      double tmp=(1.0-damping) / numNodes;
      #pragma omp parallel for
      for (int i = 0; i < numNodes; ++i){
        score_new[i]=(damping * score_new[i]) + tmp;
      }
      //  score_new[vi] += sum over all nodes v in graph with no outgoing edges
      //                     { damping * score_old[v] / numNodes }
      double sum=0;
      tmp=damping/numNodes;
      #pragma omp parallel for reduction (+:sum)
      for (int v=0; v<numNodes; v++) {
        if(outgoing_size(g,v)==0){
          sum+=score_old[v]*tmp;
        }
      }
      #pragma omp parallel for
      for (int i = 0; i < numNodes; ++i){
        score_new[i]+=sum;
      }
       // compute how much per-node scores have changed
       // quit once algorithm has converged
       //global_diff = sum over all nodes vi { abs(score_new[vi] - score_old[vi]) };
       double global_diff=0;
       #pragma omp parallel for reduction(+:global_diff)
       for (int i = 0; i < numNodes; ++i){
        global_diff+=abs(score_new[i] - score_old[i]);
       }
       converged = (global_diff < convergence);
      score_old=score_new;
     }
    #pragma omp parallel for
    for (int i = 0; i < numNodes; ++i){
      solution[i]=score_old[i];
    }

}
