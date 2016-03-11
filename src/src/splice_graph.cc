#include "splice_graph.h"
#include "draw.h"

#include <boost/graph/breadth_first_search.hpp>
#include <fstream>

using namespace boost_graph;

int boost_graph::build_splice_graph(splice_graph &gr, const string &file)
{
	ifstream fin(file.c_str());
	if(fin.fail()) 
	{
		printf("open file %s error\n", file.c_str());
		return 0;
	}

	char line[10240];
	// get the number of vertices
	fin.getline(line, 10240, '\n');	
	int n = atoi(line);

	for(int i = 0; i < n; i++)
	{
		double weight, stddev;
		fin.getline(line, 10240, '\n');	
		stringstream sstr(line);
		sstr>>weight>>stddev;

		add_vertex(gr);
		put(get(vertex_weight, gr), i, weight);
		put(get(vertex_stddev, gr), i, stddev);
	}

	while(fin.getline(line, 10240, '\n'))
	{
		int x, y;
		double weight, stddev;
		stringstream sstr(line);
		sstr>>x>>y>>weight>>stddev;

		assert(x != y);
		assert(x >= 0 && x < num_vertices(gr));
		assert(y >= 0 && y < num_vertices(gr));

		PEB p = add_edge(x, y, gr);
		put(get(edge_weight, gr), p.first, weight);
		put(get(edge_stddev, gr), p.first, stddev);
	}

	fin.close();
	return 0;
}

int boost_graph::write_splice_graph(const splice_graph &gr, const string &file)
{
	ofstream fin(file.c_str());
	if(fin.fail()) 
	{
		printf("open file %s error\n", file.c_str());
		return 0;
	}
	
	fin<<fixed;
	fin.precision(2);
	int n = num_vertices(gr);
	
	fin<<n<<endl;
	for(int i = 0; i < n; i++)
	{
		double weight = get(get(vertex_weight, gr), i);
		double stddev = get(get(vertex_stddev, gr), i);
		fin<<weight<<" "<<stddev<<endl;
	}

	edge_iterator it1, it2;
	for(tie(it1, it2) = edges(gr); it1 != it2; it1++)
	{
		int s = source(*it1, gr);
		int t = target(*it1, gr);
		double weight = get(get(edge_weight, gr), *it1);
		double stddev = get(get(edge_stddev, gr), *it1);
		fin<<s<<" "<<t<<" "<<weight<<" "<<stddev<<endl;
	}

	fin.close();
	return 0;
}

int boost_graph::draw_splice_graph(const splice_graph &gr, const string &file, double len)
{
	ofstream fout(file.c_str());
	if(fout.fail())
	{
		printf("open file %s error.\n", file.c_str());
		return 0;
	}

	draw_header(fout);

	fout<<"\\def\\len{"<<len<<"cm}\n";

	// draw vertices
	char sx[1024];
	char sy[1024];
	double pos = 0;
	for(int i = 0; i < num_vertices(gr); i++)
	{
		int d = in_degree(i, gr) + out_degree(i, gr);
		if(d == 0) continue;

		pos++;

		sprintf(sx, "s%d", i);
		fout.precision(0);
		fout<<fixed;
		fout<<"\\node[mycircle, \\colx, draw, label = below:{";
		//fout<< get(get(vertex_weight, gr), i) << ",";
		fout<< get(get(vertex_weight, gr), i);
		fout<< "}] ("<<sx<<") at ("<<pos<<" *\\len, 0.0) {"<<i<<"};\n";
	}

	// draw edges
	adj_iterator ai1, ai2;
	for(int i = 0; i < num_vertices(gr); i++)
	{
		for(tie(ai1, ai2) = adjacent_vertices(i, gr); ai1 != ai2; ai1++)
		{
			int j = *ai1;
			assert(i < j);

			string s;
			char buf[1024];
			out_edge_iterator oi1, oi2;
			int cnt = 0;
			for(tie(oi1, oi2) = edge_range(i, j, gr); oi1 != oi2; oi1++)
			{
				double w = get(get(edge_weight, gr), *oi1);
				if(cnt == 0) sprintf(buf, "%.0lf", w);
				else sprintf(buf, ",%.0lf", w);
				s.append(buf);
				cnt++;
			}

			sprintf(sx, "s%d", i);
			sprintf(sy, "s%d", j);

			double bend = -40;
			if(i + 1 == j) bend = 0;

			string line = "";
			if(cnt == 1) line = "line width = 0.02cm,";
			else line = "thin, double,";
			fout<<"\\draw[->,"<< line.c_str() <<"\\colx, bend right = "<< bend <<"] ("<<sx<<") to node {";
			//fout<< get(get(edge_weight, gr), *it1) <<",";
			fout<< s.c_str() <<"} ("<<sy<<");\n";
		}
	}

	draw_footer(fout);

	fout.close();
	return 0;
}

int boost_graph::simulate_splice_graph(splice_graph &gr, int n, int m)
{
	gr.clear();
	for(int i = 0; i < n; i++)
	{
		add_vertex(gr);
		put(get(vertex_weight, gr), i, 1);
		put(get(vertex_stddev, gr), i, 1);
	}

	for(int i = 0; i < m; i++)
	{
		int s = rand() % (n - 1);
		int t = s + 1 + rand() % (n - s - 1);
		PEB p = add_edge(s, t, gr);
		put(get(edge_weight, gr), p.first, 1);
		put(get(edge_stddev, gr), p.first, 1);
	}
	return 0;
}

int boost_graph::compute_num_paths(const splice_graph &gr)
{
	vector<int> table;
	table.resize(num_vertices(gr), 0);
	table[0] = 1;
	int n = num_vertices(gr);
	for(int i = 1; i < n; i++)
	{
		in_edge_iterator it1, it2;
		for(tie(it1, it2) = in_edges(i, gr); it1 != it2; it1++)
		{
			int s = source(*it1, gr);
			int t = target(*it1, gr);
			assert(t == i);
			assert(s < i);
			table[t] += table[s];
		}
	}
	return table[n - 1];
}

int boost_graph::get_edge_weights(const splice_graph &gr, MED &med)
{
	med.clear();
	edge_iterator it1, it2;
	for(tie(it1, it2) = edges(gr); it1 != it2; it1++)
	{
		double w = get(get(edge_weight, gr), *it1);
		med.insert(PED(*it1, w));
	}
	return 0;
}

int boost_graph::set_edge_weights(splice_graph &gr, const MED &med)
{
	edge_iterator it1, it2;
	for(tie(it1, it2) = edges(gr); it1 != it2; it1++)
	{
		MED::const_iterator it = med.find(*it1);
		put(get(edge_weight, gr), *it1, it->second);
	}
	return 0;
}

int boost_graph::get_vertex_weights(const splice_graph &gr, vector<double> &v)
{
	v.resize(num_vertices(gr), 0);
	for(int i = 0; i < v.size(); i++)
	{
		double w = get(get(vertex_weight, gr), i);
		v[i] = w;
	}
	return 0;
}

int boost_graph::set_vertex_weights(splice_graph &gr, const vector<double> &v)
{
	assert(v.size() == num_vertices(gr));
	for(int i = 0; i < v.size(); i++)
	{
		put(get(vertex_weight, gr), i, v[i]);
	}
	return 0;
}

int boost_graph::get_edge_indices(const splice_graph &gr, VE &i2e, MEI &e2i)
{
	i2e.clear();
	e2i.clear();
	int index = 0;
	edge_iterator it1, it2;
	for(tie(it1, it2) = edges(gr); it1 != it2; it1++)
	{
		e2i.insert(PEI(*it1, index));
		i2e.push_back(*it1);
		index++;
	}
	return 0;
}

bool boost_graph::check_nested_splice_graph(const splice_graph &gr)
{
	for(int i = 0; i < num_vertices(gr); i++)
	{
		out_edge_iterator it1, it2;
		for(tie(it1, it2) = out_edges(i, gr); it1 != it2; it1++)
		{
			int j = target(*it1, gr);
			assert(j > i);
			for(int k = i + 1; k < j; k++)
			{
				if(check_directed_path(gr, i, k) == false) continue;
				if(check_directed_path(gr, k, j) == false) continue;
				out_edge_iterator it3, it4;
				for(tie(it3, it4) = out_edges(k, gr); it3 != it4; it3++)
				{
					int l = target(*it3, gr);
					assert(l > k);
					if(l <= j) continue;
					
					if(check_directed_path(gr, j, l) == false) continue;

					return false;
				}
			}
		}
	}
	return true;
}

bool boost_graph::check_directed_path(const splice_graph &gr, int s, int t)
{
	// assume DAG
	assert(s < t);
	vector<bool> closed;
	closed.resize(num_vertices(gr), false);
	vector<int> open;
	open.push_back(s);
	int p = 0;

	while(p < open.size())
	{
		int x = open[p];
		p++;
		if(closed[x] == true) continue;
		closed[x] = true;

		out_edge_iterator it1, it2;
		for(tie(it1, it2) = out_edges(x, gr); it1 != it2; it1++)
		{
			int y = target(*it1, gr);
			if(y == t) return true;
			if(y < t) open.push_back(y);
		}
	}
	return false;
}

bool boost_graph::check_fully_reachable_from_start(const splice_graph &gr)
{
	// assume DAG
	vector<bool> closed;
	closed.resize(num_vertices(gr), false);
	vector<int> open;
	open.push_back(0);
	int p = 0;

	while(p < open.size())
	{
		int x = open[p];
		p++;
		if(closed[x] == true) continue;
		closed[x] = true;

		out_edge_iterator it1, it2;
		for(tie(it1, it2) = out_edges(x, gr); it1 != it2; it1++)
		{
			int y = target(*it1, gr);
			if(closed[y] == true) continue;
			open.push_back(y);
		}
	}

	for(int i = 0; i < closed.size(); i++)
	{
		if(closed[i] == false) return false;
	}

	return true;
}

bool boost_graph::check_fully_reachable_to_end(const splice_graph &gr)
{
	// assume DAG
	vector<bool> closed;
	closed.resize(num_vertices(gr), false);
	vector<int> open;
	open.push_back(num_vertices(gr) - 1);
	int p = 0;

	while(p < open.size())
	{
		int x = open[p];
		p++;
		if(closed[x] == true) continue;
		closed[x] = true;

		in_edge_iterator it1, it2;
		for(tie(it1, it2) = in_edges(x, gr); it1 != it2; it1++)
		{
			int y = source(*it1, gr);
			if(closed[y] == true) continue;
			open.push_back(y);
		}
	}

	for(int i = 0; i < closed.size(); i++)
	{
		if(closed[i] == false) return false;
	}

	return true;
}

bool boost_graph::check_fully_connected(const splice_graph &gr)
{
	bool b1 = check_fully_reachable_from_start(gr);
	bool b2 = check_fully_reachable_to_end(gr);
	if(b1 && b2) return true;
	else return false;
}

int boost_graph::bfs_distance(const splice_graph &gr, int s, vector<int> &v)
{
	v.assign(num_vertices(gr), 0);
	breadth_first_search(gr, s, visitor(make_bfs_visitor(record_distances(&v[0], on_tree_edge()))));
	return 0;
}

int boost_graph::test_bfs_distance()
{
	splice_graph gr;
	int N = 5;
	for(int i = 0; i < N; i++) add_vertex(gr);
	add_edge(0, 1, gr);
	add_edge(0, 2, gr);
	add_edge(1, 2, gr);
	add_edge(1, 3, gr);
	add_edge(1, 4, gr);
	add_edge(2, 3, gr);
	add_edge(3, 4, gr);
	add_edge(3, 4, gr);
	add_edge(2, 1, gr);
	add_edge(1, 0, gr);

	vector<int> v;
	bfs_distance(gr, 1, v);

	for(int i = 0; i < v.size(); i++)
	{
		printf("from %d to %d, distance = %d\n", 1, i, v[i]);
	}
	return 0;
}

int boost_graph::test_remove_edge()
{
	splice_graph gr;
	add_vertex(gr);
	add_vertex(gr);
	PEB p1 = add_edge(0, 1, gr);
	PEB p2 = add_edge(0, 1, gr);
	PEB p3 = add_edge(0, 1, gr);
	remove_edge(p1.first, gr);
	remove_edge(p2.first, gr);
	remove_edge(p3.first, gr);
	return 0;
}
