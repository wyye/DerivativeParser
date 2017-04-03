#include "parser.h"
#include "tree_node.h"

#include <string>
#include <sstream>
#include <iostream>

auto main() -> decltype(10) {
	Parser p;
	
	//std::string expr = "sin(2*sin(x))";
	//std::string expr = "x^2";
	//std::string expr = "2*(3*xx)*xx";
	std::string expr = "3+4*5*exp(3*xx)-2^3/xx+(-10)-(2.02+31)+foo(3,xx,y3y)";
	//std::string expr = "-10 + (-4+(+10)) * 2 - (-[foo(3*x+5*y), x]) - 2*(-3-4-foo())";
	std::istringstream iss(expr);
	
	Node* tree;
	Node* tree_d;
	Node* tree_d_s;
	
	try {
		p.setInput(iss);
		tree = p.getG();
		
		tree->prettyPrint(std::cout);
		std::cout << std::endl;
	
		std::ofstream ofs("graph.dot");
		ofs << "digraph {\n";
		tree->toDotGraph(ofs);
		ofs << "}\n";
		ofs.close();

		tree_d = tree->takeDerivative("xx");
		
		tree_d->prettyPrint(std::cout);
		std::cout << std::endl;
		
		std::ofstream ofsd("graph_d.dot");
		ofsd << "digraph {\n";
		tree_d->toDotGraph(ofsd);
		ofsd << "}\n";
		ofsd.close();
		
		tree_d_s = tree_d->simplify();
		
		tree_d_s->prettyPrint(std::cout);
		std::cout << std::endl;
		
		std::ofstream ofsds("graph_d_s.dot");
		ofsds << "digraph {\n";
		tree_d_s->toDotGraph(ofsds);
		ofsds << "}\n";
		ofsds.close();
		
		delete tree;
		delete tree_d;
		delete tree_d_s;
		
	} catch (const LexerException& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Line: " << e.getLine() << std::endl;
		std::cout << "Pos: " << e.getPos() << std::endl;
	} catch (const ParserException& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Line: " << e.getLine() << std::endl;
		std::cout << "Pos: " << e.getPos() << std::endl;
	}	catch (const Exception& e) {
		std::cerr << "Unknown exception: " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown error" << std::endl;
	}
	
	return 0;
}
