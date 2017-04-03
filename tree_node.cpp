#include "tree_node.h"
#include "debug.h"
#include <cmath>

int Node::S_LAST_NODE_ID = 0;

int ArgsNode ::S_PRIORITY = 50;
int AddNode  ::S_PRIORITY = 100;
int SubNode  ::S_PRIORITY = 100;
int MinusNode::S_PRIORITY = 100;
int MulNode  ::S_PRIORITY = 200;
int DivNode  ::S_PRIORITY = 200;
int PowNode  ::S_PRIORITY = 300;
int FuncNode ::S_PRIORITY = 400;
int IdNode   ::S_PRIORITY = 500;
int NumNode  ::S_PRIORITY = 500;
int DerivativeNode::S_PRIORITY = 500;

Node* FuncNode::takeDerivative(const std::string& var) const {
	
	ArgsNode* args = dynamic_cast<ArgsNode*>(m_right);
	ASSERT(args != nullptr);
	
	Node* prev_sum = nullptr;
	
	IdNode* new_id = dynamic_cast<IdNode*>(m_left->clone());
	ASSERT(new_id != nullptr);
		
	if (new_id->getValue() == "sin") {
		return 
			new MulNode(
				args->getParam(0)->takeDerivative(var),
				new FuncNode(new IdNode("cos"), dynamic_cast<ArgsNode*>(args->clone()))
			);
	} else if (new_id->getValue() == "cos") {
		
		return 
			new MinusNode(
				new MulNode(
					args->getParam(0)->takeDerivative(var),
					new FuncNode(
						new IdNode("cos"), 
						dynamic_cast<ArgsNode*>(args->clone())
					)
				)
			);
			
	} if (new_id->getValue() == "exp") {
		return
			new MulNode(
				args->getParam(0)->takeDerivative(var),
				new FuncNode(new IdNode("exp"), dynamic_cast<ArgsNode*>(args->clone()))
			);
	} else {
		delete new_id;
		

		/*ArgsNode* args = dynamic_cast<ArgsNode*>(m_right);
		ASSERT(args != nullptr);
		
		Node* prev_sum = nullptr;*/
		
		for (size_t i = 0; i < args->getSize(); ++i) {
			Node* mul = args->getParam(i)->takeDerivative(var);
			IdNode* new_m_left = dynamic_cast<IdNode*>(m_left->clone());
			
			//ASSERT(new_m_left != nullptr);
			Node* expr = new FuncNode(new_m_left, new ArgsNode(*args));
			Node* dn = new DerivativeNode(expr, args->getParam(i)->clone());
			Node* sum_part = new MulNode(mul, dn);
			
			if (prev_sum == nullptr) {
				prev_sum = sum_part;
			} else {
				prev_sum = new AddNode(prev_sum, sum_part);
			}
		}
		
		return prev_sum;
	}
}

Node* DerivativeNode::takeDerivative(const std::string& var) const {
	VASSERT(0, "Not implemented");
	return nullptr;
}

Node* PowNode::takeDerivative(const std::string& var) const {
	
	Node* b_1 = new SubNode(m_right->clone(), new NumNode(1)); 
	Node* a_pow_b_1 = new PowNode(m_left->clone(), b_1);
	
	
	Node* b_da = new MulNode(m_left->takeDerivative(var), m_right->clone());
	Node* a_db = new MulNode(m_left->clone(), m_right->takeDerivative(var));
	
	ArgsNode* args = new ArgsNode();
	args->push_back(m_left->clone());
	Node* func = new FuncNode(new IdNode("log"), args);
	Node* a_db_log = new MulNode(func, a_db);
	Node* sum = new AddNode(b_da, a_db_log);
	return new MulNode(a_pow_b_1, sum);
}

Node* NumNode::takeDerivative(const std::string&) const {
	return new NumNode(0);
}

Node* IdNode::takeDerivative(const std::string& var) const {
	if (m_val == var) {
		return new NumNode(1);
	} else {
		return new NumNode(0);
	}
}

Node* MinusNode::takeDerivative(const std::string& var) const {
	return m_node->takeDerivative(var);
}

Node* DivNode::takeDerivative(const std::string& var) const {
	Node* n1 = new MulNode(m_left->takeDerivative(var), m_right->clone());
	Node* n2 = new MulNode(m_left->clone(), m_right->takeDerivative(var));
	Node* sub = new SubNode(n1, n2);
	Node* pow = new PowNode(m_right->clone(), new NumNode(2));
	return new DivNode(sub, pow);
}

void BinaryNode::prettyPrint(std::ostream& out) const {
	bool needRightPar = false;
	bool needLeftPar = false;
	
	if (
		m_left->getPriority() < getPriority() ||
		(m_left->getPriority() == getPriority() && 
		isInstanceOf<MinusNode>(m_left))
	) {
		needLeftPar = true;
	}
	
	if (needLeftPar) out << "(";
	m_left->prettyPrint(out);
	if (needLeftPar) out << ")";
	
	out << getSym();
	
	if (
		m_right->getPriority() < getPriority() ||
		
		(m_right->getPriority() == getPriority() &&
			(isInstanceOf<SubNode>(this) || 
			isInstanceOf<DivNode>(this) || 
			isInstanceOf<MinusNode>(m_right))
		)
	) {
		needRightPar = true;
	}
	
	if (needRightPar) out << "(";
	m_right->prettyPrint(out);
	if (needRightPar) out << ")";
}

Node* AddNode::simplify() const {
	Node* left = m_left->simplify();
	Node* right = m_right->simplify();
	
	NumNode* cast_left = dynamic_cast<NumNode*>(left);
	NumNode* cast_right = dynamic_cast<NumNode*>(right);
	
	if (cast_left != nullptr && cast_right != nullptr) {
		NumNode* ret = new NumNode(cast_left->getValue() + cast_right->getValue());
		delete left;
		delete right;
		return ret;
	} else if (cast_left != nullptr && std::fabs(cast_left->getValue()) < EPS) {
		delete left;
		return right;
	} else if (cast_right != nullptr && std::fabs(cast_right->getValue()) < EPS) {
		delete right;
		return left;
	}
	
	return new AddNode(left, right);
}

Node* SubNode::simplify() const {
	Node* left = m_left->simplify();
	Node* right = m_right->simplify();
	
	NumNode* cast_left = dynamic_cast<NumNode*>(left);
	NumNode* cast_right = dynamic_cast<NumNode*>(right);
	
	if (cast_left != nullptr && cast_right != nullptr) {
		double val = cast_left->getValue() - cast_right->getValue();
		Node* ret;
		if (val < 0) {
			ret = new MinusNode(new NumNode(-val));
		} else {
			ret = new NumNode(val);
		}
		delete left;
		delete right;
		return ret;
	} else if (cast_left != nullptr && std::fabs(cast_left->getValue()) < EPS) {
		delete left;
		return new MinusNode(right);
	} else if (cast_right != nullptr && std::fabs(cast_right->getValue()) < EPS) {
		delete right;
		return left;
	}
	
	return new SubNode(left, right);
}

Node* MinusNode::simplify() const {
	Node* node = m_node->simplify();
	NumNode* cast_node = dynamic_cast<NumNode*>(node);
	if (cast_node != nullptr && std::fabs(cast_node->getValue()) < EPS) {
		delete node;
		return new NumNode(0);
	}
	return new MinusNode(node);
}

Node* MulNode::simplify() const {
	Node* left = m_left->simplify();
	Node* right = m_right->simplify();
	
	NumNode* cast_left = dynamic_cast<NumNode*>(left);
	NumNode* cast_right = dynamic_cast<NumNode*>(right);
	
	if (cast_left != nullptr && cast_right != nullptr) {
		NumNode* ret = new NumNode(cast_left->getValue() * cast_right->getValue());
		delete left;
		delete right;
		return ret;
	} else if (cast_left != nullptr) {
		if (std::fabs(cast_left->getValue()) < EPS) {
			delete right;
			delete left;
			return new NumNode(0);
		} else if (std::fabs(cast_left->getValue() - 1) < EPS) {
			delete left;
			return right;
		}
	} else if (cast_right != nullptr) {
		if (std::fabs(cast_right->getValue()) < EPS) {
			delete left;
			delete right;
			return new NumNode(0);
		} else if (std::fabs(cast_right->getValue() - 1) < EPS) {
			delete right;
			return left;
		}
	}
	
	return new MulNode(left, right);
}

Node* DivNode::simplify() const {
	Node* left = m_left->simplify();
	Node* right = m_right->simplify();
	
	NumNode* cast_left = dynamic_cast<NumNode*>(left);
	NumNode* cast_right = dynamic_cast<NumNode*>(right);
	
	if (cast_left != nullptr && cast_right != nullptr) {
		NumNode* ret = new NumNode(cast_left->getValue() / cast_right->getValue());
		delete left;
		delete right;
		return ret;
	} else if (cast_left != nullptr && std::fabs(cast_left->getValue()) < EPS) {
		delete right;
		delete left;
		return new NumNode(0);
	} 
	
	return new DivNode(left, right);
}

Node* PowNode::simplify() const {
	Node* left = m_left->simplify();
	Node* right = m_right->simplify();
	
	NumNode* cast_left = dynamic_cast<NumNode*>(left);
	NumNode* cast_right = dynamic_cast<NumNode*>(right);
	
	if (cast_left != nullptr && cast_right != nullptr) {
		NumNode* ret = new NumNode(std::pow(cast_left->getValue() , cast_right->getValue()));
		delete left;
		delete right;
		return ret;
	} else if (cast_left != nullptr && std::fabs(cast_left->getValue()) < EPS) {
		delete right;
		delete left;
		return new NumNode(0);
	} else if (cast_right != nullptr) {
		if (std::fabs(cast_right->getValue()) < EPS) {
			delete left;
			delete right;
			return new NumNode(1);
		} else if (std::fabs(cast_right->getValue()-1) < EPS) {
			delete right;
			return left;
		}
	} 
	
	return new PowNode(left, right);
}

Node* ArgsNode::simplify() const {
	ArgsNode* n = new ArgsNode();
	for (auto v : m_params) {
		n->push_back(v->simplify());
	}
	
	return n;
}

Node* NumNode::simplify() const {
	return clone();
}

Node* IdNode::simplify() const {
	return clone();
}

Node* FuncNode::simplify() const {
	ArgsNode* args = dynamic_cast<ArgsNode*>(m_right->simplify());
	ASSERT(args != nullptr);
	IdNode* left = dynamic_cast<IdNode*>(m_left->clone());
	ASSERT(left != nullptr);
	return new FuncNode(left, args);
}

Node* DerivativeNode::simplify() const {
	return new DerivativeNode(m_left->simplify(), m_right->simplify());
}
