#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <vector>
#include <string>
#include <fstream>

#include "utils.h"
#include "debug.h"

struct Node {
	
	Node() :
		m_id(S_LAST_NODE_ID++)
	{
	}
	
	Node(const Node&) :
		m_id(S_LAST_NODE_ID++)
	{
		
	}
	
	Node(Node&&) :
		m_id(S_LAST_NODE_ID++)
	{
	}
	
	virtual ~Node() {};
	
	virtual
	void prettyPrint(std::ostream& out) const = 0;
	
	virtual
	int getPriority() const = 0;
	
	virtual 
	void toDotGraph(std::ostream& out) const = 0;
	
	virtual
	std::string getDotRepr() const = 0;
	
	int getId() const {
		return m_id;
	}
	
	virtual
	Node* clone() const = 0;
	
	virtual
	Node* takeDerivative(const std::string& var) const = 0;
	
	virtual
	Node* simplify() const = 0;
	
private:
	
	int m_id;
	
	static int S_LAST_NODE_ID;
};

struct BinaryNode : public Node {
	
	BinaryNode(Node* left, Node* right) :
		Node(),
		m_left(left),
		m_right(right)
	{}
	
	BinaryNode(const BinaryNode& other) :
		BinaryNode(other.m_left->clone(), other.m_right->clone())
	{
	}
	
	BinaryNode(BinaryNode&& other) :
		BinaryNode(other.m_left, other.m_right)
	{
		other.m_left = nullptr;
		other.m_right = nullptr;
	}
	
	Node* getLeft() const {
		return m_left;
	}
	
	Node* getRight() const {
		return m_right;
	}
	
	void setLeft(Node* left) {
		m_left = left;
	}
	
	void setRight(Node* right) {
		m_right = right;
	}
	
	virtual
	void prettyPrint(std::ostream& out) const override;
	
	virtual
	std::string getSym() const = 0;
	
	virtual
	void toDotGraph(std::ostream& out) const override {
		out << getId() << " [label=\"" << getDotRepr() << "\"];\n";
		
		out << getId();
		out << " -> ";
		out << m_left->getId();
		out << ";\n";
		m_left->toDotGraph(out);
		
		out << getId();
		out << " -> ";
		out << m_right->getId();
		out << ";\n";
		m_right->toDotGraph(out);
	}
	
	virtual
	std::string getDotRepr() const override {
		return getSym();
	}
	
	
	virtual 
	~BinaryNode() {
		if (m_left != nullptr) delete m_left;
		if (m_right != nullptr) delete m_right;
	}
	
	
protected:
	
	Node* m_left;
	Node* m_right;
};

struct AddNode : public BinaryNode {
	AddNode(Node* left, Node* right) :
		BinaryNode(left, right) 
	{}
	
	AddNode(const AddNode& cur) = default;
	AddNode(AddNode&& cur) = default;
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	std::string getSym() const override { return "+"; }
	
	virtual
	Node* clone() const override {
		return new AddNode(*this);
	}
	
	virtual
	Node* takeDerivative(const std::string& var) const override {
		return new AddNode(
			m_left->takeDerivative(var),
			m_right->takeDerivative(var));
	}
	
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;
};

struct SubNode : public BinaryNode {
	SubNode(Node* left, Node* right) :
		BinaryNode(left, right) 
	{}
	
	SubNode(const SubNode& cur) = default;
	SubNode(SubNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new SubNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	std::string getSym() const override { return "-"; }
	
	virtual
	Node* takeDerivative(const std::string& var) const override {
		return new SubNode(
			m_left->takeDerivative(var),
			m_right->takeDerivative(var));
	}
	
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;
};

struct MulNode : public BinaryNode {
	MulNode(Node* left, Node* right) :
		BinaryNode(left, right) 
	{}
	
	MulNode(const MulNode& cur) = default;
	MulNode(MulNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new MulNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	std::string getSym() const override { return "*"; }
	
	virtual
	Node* takeDerivative(const std::string& var) const override {
		Node* n1 = new MulNode(m_left->takeDerivative(var), m_right->clone());
		Node* n2 = new MulNode(m_left->clone(), m_right->takeDerivative(var));
		return new AddNode(n1, n2);
	}
	
	/*
	Node* takeDer (var)
	{
		return &( right->clone() * left->der() + ....;
		
		return ANSWER (cL * dR + dL * cR); dsl
	}
	*/
	
	virtual
	Node* simplify() const override;
	
	
	static int S_PRIORITY;
};

struct DivNode : public BinaryNode {
	DivNode(Node* left, Node* right) :
		BinaryNode(left, right) 
	{}
	
	DivNode(const DivNode& cur) = default;
	DivNode(DivNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new DivNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	Node* takeDerivative(const std::string& var) const override;
	
	virtual
	std::string getSym() const override { return "/"; }
	
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;
};

struct PowNode : public BinaryNode {
	PowNode(Node* left, Node* right) :
		BinaryNode(left, right) 
	{}
	
	PowNode(const PowNode& cur) = default;
	PowNode(PowNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new PowNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	std::string getSym() const override { return "^"; }
	
	virtual
	Node* takeDerivative(const std::string& var) const override;
	
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;
};

struct ArgsNode : public Node {
	
	ArgsNode() :
		Node()
	{
	}
	
	ArgsNode(const ArgsNode& cur) :
		Node(cur)
	{
		m_params.reserve(cur.getSize());
		for (size_t i = 0; i < cur.getSize(); ++i) 
			m_params.push_back(cur.getParam(i)->clone());
	}
	
	ArgsNode(ArgsNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new ArgsNode(*this);
	}
	
	virtual
	void prettyPrint(std::ostream& out) const override {
		
		if (m_params.size() != 0) 
			m_params[0]->prettyPrint(out);
		for (std::vector<Node*>::size_type i = 1; i < m_params.size(); ++i) {
			out << ", ";
			m_params[i]->prettyPrint(out);
		}
		
	}
	
	~ArgsNode() {
		for (auto v : m_params) {
			if (v != nullptr) delete v;
		}
	}
	
	void push_back(Node* arg) {
		m_params.push_back(arg);
	}
	
	size_t getSize() const {
		return m_params.size();
	}
	
	Node* getParam(size_t index) const {
		ASSERT(index < m_params.size());
		return m_params[index];
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	std::string getDotRepr() const override {
		return "_args";
	}
	
	virtual
	void toDotGraph(std::ostream& out) const override {
		out << getId() << " [label=\"" << getDotRepr() << "\"];\n";
		
		for (decltype(m_params)::size_type i = 0; i != m_params.size(); ++i) {
			out << getId();
			out << " -> ";
			out << m_params[i]->getId();
			out << "[label=" << i << "];\n";
			m_params[i]->toDotGraph(out);
		}
	}
	
	virtual
	Node* takeDerivative(const std::string& var) const override {
		VASSERT(0, "Trying to take derivative from arguments");
		return nullptr;
	}
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;
	
private:
	std::vector<Node*> m_params;
};

struct NumNode : public Node {
	NumNode(double val) :
		Node(),
		m_val(val)
	{
	}
	
	NumNode(const NumNode& cur) = default;
	NumNode(NumNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new NumNode(*this);
	}
	
	double getValue() const {
		return m_val;
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	void prettyPrint(std::ostream & out) const override {
		if (m_val < 0) out << "(";
		out << m_val;
		if (m_val < 0) out << ")";
	}
	
	virtual
	void toDotGraph(std::ostream & out) const override {
		out << getId() << " [label=\"" << getDotRepr() << "\"];\n";
	}
	
	virtual
	std::string getDotRepr() const override {
		return toString(m_val);
	}
	
	virtual
	Node* takeDerivative(const std::string&) const override;
	
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;

private:
	double m_val;
};

struct IdNode : public Node {
	
	IdNode(const std::string& val) :
		Node(),
		m_val(val)
	{}
	
	IdNode(std::string&& val) :
		Node(),
		m_val(std::move(val))
	{}
	
	IdNode(const IdNode& cur) = default;
	IdNode(IdNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new IdNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	void prettyPrint(std::ostream & out) const override {
		out << m_val;
	}
	
	virtual
	std::string getDotRepr() const override {
		return toString(m_val);
	}
	
	virtual
	void toDotGraph(std::ostream & out) const override {
		out << getId() << " [label=\"" << getDotRepr() << "\"];\n";
	}
	
	virtual
	Node* takeDerivative(const std::string& var) const override;
	
	virtual
	Node* simplify() const override;
	
	std::string getValue() const {
		return m_val;
	}
	
	static int S_PRIORITY;
	
private:
	std::string m_val;
};

struct MinusNode : public Node {
	MinusNode(Node* node) :
		Node(),
		m_node(node)
	{}
	
	MinusNode(const MinusNode& other) :
		Node(),
		m_node(other.m_node->clone())
	{
	}
	
	MinusNode(MinusNode&& other) :
		Node(),
		m_node(other.m_node)
	{
		other.m_node = nullptr;
	}
	
	virtual
	Node* clone() const override {
		return new MinusNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	static int S_PRIORITY;
	
	virtual
	void prettyPrint(std::ostream & out) const override {
		out << getDotRepr();
		if (m_node->getPriority() <= getPriority()) out << "(";
		m_node->prettyPrint(out);
		if (m_node->getPriority() <= getPriority())	out << ")";
	}
	
	virtual 
	std::string getDotRepr() const override {
		return "-";
	}
	
	virtual
	void toDotGraph(std::ostream& out) const override {
		out << getId() << " [label=\"" << getDotRepr() << "\"];\n";
		
		out << getId();
		out << " -> ";
		out << m_node->getId();
		out << ";\n";
		m_node->toDotGraph(out);
	}
	
	~MinusNode() {
		if (m_node != nullptr) delete m_node;
	}
	
	virtual
	Node* takeDerivative(const std::string& var) const override;
	
	virtual
	Node* simplify() const override;
	
private:
	
	Node* m_node;
};

struct FuncNode : public BinaryNode {
	FuncNode(IdNode* left, ArgsNode* right) :
		BinaryNode(left, right) 
	{}
	
	FuncNode(const FuncNode& cur) = default;
	FuncNode(FuncNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new FuncNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	std::string getSym() const override { return ""; }
	
	virtual
	std::string getDotRepr() const override {
		return "_func";
	}
	
	virtual
	Node* takeDerivative(const std::string& var) const override;
	
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;
};

struct DerivativeNode : public BinaryNode {
	DerivativeNode(Node* left, Node* right) :
		BinaryNode(left, right)
	{
	}
	
	DerivativeNode(const DerivativeNode& cur) = default;
	DerivativeNode(DerivativeNode&& cur) = default;
	
	virtual
	Node* clone() const override {
		return new DerivativeNode(*this);
	}
	
	virtual 
	int getPriority() const override {
		return S_PRIORITY;
	}
	
	virtual
	void prettyPrint(std::ostream& out) const override {		
		out << "[";
		m_left->prettyPrint(out);
		out << ", ";
		m_right->prettyPrint(out);
		out << "]";
	}
	
	virtual
	std::string getSym() const override {
		ASSERT(0);
		return "";
	}
	
	virtual
	std::string getDotRepr() const override {
		return "_deriv";
	}
	
	virtual
	void toDotGraph(std::ostream& out) const override {
		out << getId() << " [label=\"" << getDotRepr() << "\"];\n";
		
		out << getId();
		out << " -> ";
		out << m_left->getId();
		out << "[label=\"expr\"];\n";
		
		m_left->toDotGraph(out);
		
		out << getId();
		out << " -> ";
		out << m_right->getId();
		out << "[label=\"arg\"];\n";
		
		m_right->toDotGraph(out);
	}
	
	virtual
	Node* takeDerivative(const std::string& var) const override;
	
	virtual
	Node* simplify() const override;
	
	static int S_PRIORITY;
};

#endif
