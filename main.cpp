/*
	Kris Koeckritz
	last mod: 04/01/18
*/

/*
#include <map>
#include <set> 
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <bitset>
#include <numeric>
#include <cstdio>
#include <limits>
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <functional>
#include <utility>

using namespace std;

vector<vector<int>> computeEditDistanceMatrix(const string& s1, const string& s2)
{
	vector<vector<int>> result{};

	// initialize vector matrix; first row...
	vector<int> first_row{};
	for (int i = 0; i <= s2.size(); i++)
	{
		first_row.push_back(i);
	}
	result.push_back(first_row);

	// ...and subsequent rows
	for (int i = 1; i <= s1.size(); i++)
	{
		vector<int> new_row(s2.size() + 1, -1);
		new_row[0] = i;
		result.push_back(new_row);
	}

	// fill matrix
	for (int j = 1; j <= s2.size(); j++)
	{
		for (int i = 1; i <= s1.size(); i++)
		{
			int cost = 0;

			if (s1[i - 1] != s2[j - 1])
			{
				cost = 1;
			}

			// set current "cell" to MIN(u + 1, l + 1, ul + cost)
			int up = result[i][j - 1] + 1;
			int left = result[i - 1][j] + 1;
			int up_left = result[i - 1][j - 1] + cost;

			result[i][j] = min({ up, left, up_left });
		}
	}
	return result;
}

int computeEditDistance(const string& s1, const string& s2)
{
	vector<vector<int>> mat = computeEditDistanceMatrix(s1, s2);
	int result = -1;
	int len1 = s1.size();
	int len2 = s2.size();

	result = mat[len1][len2];

	return result;
}

void outputEditDistanceMatrix(const string& s1, const string& s2)
{
	vector<vector<int>> result = computeEditDistanceMatrix(s1, s2);
	for (auto row : result)
	{
		for (int i : row)
		{
			cout << i << " ";
		}

		cout << endl;
	}
}

vector<vector<string>> spellCheck(vector<string>& dict, vector<vector<string>>& lines)
{
	vector<vector<string>> out_lines{};
	ifstream in_corr_file;
	ofstream out_corr_file;
	string corr_name;
	
	for (auto l : lines)
	{
		vector<string> out_words;
		//int corr_key = 0;
		
		for (string s : l)
		{
			map<int, pair<string, int>> corrections;
			string word;
			string out_word;
			char punc = '\0';
			bool word_good = false;
			
			// check for punctuation; remove and store if necessary
			if ((s.back() == ',') || (s.back() == '.') || (s.back() == '?') || (s.back() == '!'))
			{
				word = s.substr(0, s.size() - 1);
				out_word = word;
				punc = s.back();
			}
			else
			{
				word = s;
				out_word = s;
			}
			
			// if existing correction file, no need to run algorithm
			in_corr_file.open(word + ".wrd");
			if (in_corr_file)
			{
				string corr;
				int dist;
				int corr_count = 0;
				while (in_corr_file.good())
				{
					in_corr_file >> corr >> dist;
					corrections[corr_count].first = corr;
					corrections[corr_count].second = dist;
					corr_count++;
				}
			}
			else
			{
				// build correction map
				for (string d_word : dict)
				{
					int comp_dist = computeEditDistance(d_word, word);

					// if d_word is not an exact fit, run Levenschtein 
					if (comp_dist > 0)
					{
						int unused_key = -1;
						int max_key = -1;
						int max_dist = -1;
						pair<string, int> dict_pair{d_word, comp_dist};

						for (int i = 0; i < 10; i++)
						{
							if (corrections.find(i) == corrections.end())
							{
								// key is empty, can be used
								unused_key = i;
								break;
							}
							else
							{
								// key is in use
								if (max_dist < corrections[i].second)
								{
									max_key = i;
									max_dist = corrections[i].second;
								}
							}
						}

						// insert if possible
						if (corrections.size() >= 10)
						{
							// map is full; if dist smaller than max_dist, replace that element
							if (comp_dist < corrections[max_key].second)
							{
								corrections[max_key] = dict_pair;
							}
						}
						else
						{
							// map is not full; insert where unused
							corrections[unused_key] = dict_pair;
						}
					}
					else
					{
						// word and dict_word match! move on to next one
						word_good = true;
						break;
					}
				}
			}

			in_corr_file.close();

			if (!word_good)
			{
				// draw correction menu
				string choice_str;
				int choice_int;

				cout << endl;
				cout << "Unknown word: " << "\"" << word << "\"" << endl;
				cout << "Select a correction: " << endl;

				cout << "1: " << "None of below are correct" << endl;

				for (int i = 0; i < corrections.size(); i++)
				{
					cout << i + 2 << ": " << corrections[i].first << endl;
				}

				cout << "Enter choice: ";
				cin >> choice_int;

				if ((choice_int < 1) || (choice_int > corrections.size()))
				{
					cout << "Please enter a number between 1 and " << corrections.size() << ": ";
					cin >> choice_int;
				}

				// set word to correction choice
				if (choice_int == 1)
				{
					cout << "Enter custom word: ";
					cin >> word;
				}
				else
				{
					word = corrections[choice_int - 2].first;
				}

				// create correction file for later use
				out_corr_file.open(out_word + ".wrd");
				for (int i = 0; i < corrections.size(); i++)
				{
					string cor_word = corrections[i].first;
					int cor_dist = corrections[i].second;
					out_corr_file << cor_word << " " << cor_dist;

					// to avoid extra newline
					if (i < corrections.size() - 1)
					{
						out_corr_file << endl;
					}
				}

				out_corr_file.close();
			}
			
			// add back punctuation if necessary
			word += punc;
			out_words.push_back(word);
		}

		// push back lines to output vect
		out_lines.push_back(out_words);
	}

	return out_lines;
}

int main()
{
	ifstream dict_file;
	ifstream i_file;
	ofstream o_file;
	string i_name;
	string o_name;
	
	vector<string> dict{};
	string dict_word;

	// load dictionary words into vector
	cout << "Loading word list... ";
	dict_file.open("words.txt");

	while (getline(dict_file, dict_word))
	{
		dict.push_back(dict_word);
	}
	cout << "Done." << endl;
	
	cout << "Input text filename: ";
	cin >> i_name;
	i_file.open(i_name);

	// if bad input filename, ask for another:
	while (!i_file.good())
	{
		cout << "Invalid file." << endl;
		cout << "Filename to correct: ";
		cin >> i_name;
		i_file.open(i_name);
	}

	cout << "Output filename: ";
	cin >> o_name;
	o_file.open(o_name);

	// check output filename for good measure
	while (!o_file.good())
	{
		cout << "Can't write to this file." << endl;
		cout << "Try another: ";
		cin >> i_name;
		o_file.open(i_name);
	}

	vector<vector<string>> lines{};
	string cur_line;
	string cur_word;
	istringstream iss;
	
	// load lines from input file into vector
	while (getline(i_file, cur_line))
	{	
		vector<string> words{};
		iss.clear();
		iss.str(cur_line);
		while (iss >> cur_word)
		{
			words.push_back(cur_word);
		}
		lines.push_back(words);
	}
	
	// spellcheck!
	vector<vector<string>> out_lines{};
	out_lines = spellCheck(dict, lines);

	// print corrected output file
	for (auto l : out_lines)
	{
		for (string s : l)
		{
			o_file << s;
		}
		o_file << endl;
	}
	
	i_file.close();
	o_file.close();

	cout << "Wrote to " + o_name << endl;
	cout << "Press any key to quit.";
	cin.get();
	
	return 0;
}
