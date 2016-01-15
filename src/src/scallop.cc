#include <cstdio>
#include <cassert>

#include "config.h"
#include "scallop.h"
#include "sam.h"

scallop::scallop()
{
}

scallop::~scallop()
{
}

int scallop::process(const char *bam_file)
{
	load(bam_file);
	solve();
	return 0;
}

int scallop::load(const char *bam_file)
{
    samFile *fn = sam_open(bam_file, "r");
    bam_hdr_t *h= sam_hdr_read(fn);
    bam1_t *b = bam_init1();

	bbase bb;
    while(sam_read1(fn, h, b) >= 0)
	{
		bam1_core_t &p = b->core;
		if((p.flag & 0x4) >= 1) continue;		// read is not mapped, TODO
		if((p.flag & 0x100) >= 1) continue;		// secondary alignment
		if(p.n_cigar < 1) continue;				// should never happen
		if(p.n_cigar > 7) continue;				// ignore hits with more than 7 cigar types
		//if(p.qual <= 4) continue;				// ignore hits with quality-score < 5
		if(bb.get_num_hits() > 0 && (bb.get_rpos() + min_bundle_gap < p.pos || p.tid != bb.get_tid()))
		{
			sgraph sg(bb);
			sgraphs.push_back(sg);

			bb.clear();

			// DEBUG
			if(sgraphs.size() >= 10) break;
		}
		bb.add_hit(h, b);
    }

    bam_destroy1(b);
    bam_hdr_destroy(h);
    sam_close(fn);

	return 0;
}

int scallop::solve()
{
	for(int i = 0; i < sgraphs.size(); i++)
	{
		sgraph &sg = sgraphs[i];
		sg.solve();

		sg.print(i);

		char s[1024];
		sprintf(s, "sgraph%d.tex", i);
		sg.draw(s);
	}
	return 0;
}
