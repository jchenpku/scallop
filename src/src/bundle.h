#ifndef __BUNDLE_H__
#define __BUNDLE_H__

#include "splice_graph.h"
#include "interval_map.h"
#include "bundle_base.h"
#include "junction.h"
#include "partial_exon.h"
#include "hyper_edge.h"
#include "path.h"
#include "super_region.h"

using namespace std;

class bundle : public bundle_base
{
public:
	bundle(const bundle_base &bb);
	virtual ~bundle();

private:
	vector<junction> junctions;		// splice junctions
	splice_graph jr;				// junction graph
	split_interval_map imap;		// interval map
	vector<super_region> srs;		// super regions
	vector<partial_exon> pexons;	// partial exons
	vector<hyper_edge> hedges;		// hyper edges

public:
	virtual int build();
	int build_splice_graph(splice_graph &gr, vector<hyper_edge> &vhe) const;
	int output_gtf(ofstream &fout, const vector<path> &paths, const string &prefix, int index) const;	
	int print_5end_coverage() const;
	int print_3end_coverage() const;
	int print(int index) const;
	int size() const;

protected:
	// check whether hits are sorted
	int check_left_ascending();
	int check_right_ascending();

	// infer boundaries
	int infer_junctions();

	// junction graph, for paired-end reads
	int build_junction_graph();
	int draw_junction_graph(const string &file);
	int search_junction_graph(int32_t p);
	int traverse_junction_graph(int s, int t, VE &ve);
	int traverse_junction_graph1(int s, int t, VE &ve);
	int traverse_junction_graph1(int s, int t);
	int test_junction_graph();

	// process hits
	int process_hits();
	int add_mapped_intervals(const hit &h, int rr);
	int add_gapped_intervals(const hit &h);

	// build partial exons
	int build_super_region(int k, super_region &sr);
	int build_super_regions();
	int build_partial_exons();
	int search_partial_exons(int32_t p);

	// super junctions and super partial_exons;
	int infer_hyper_junctions();
	int infer_hyper_edges();

	// store the corresponding pexons in each junction
	int link_partial_exons();
};

#endif
