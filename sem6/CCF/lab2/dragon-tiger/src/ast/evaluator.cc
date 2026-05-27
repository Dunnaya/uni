#include "evaluator.hh"
#include "../utils/errors.hh"

namespace ast {

int32_t Evaluator::visit(const IntegerLiteral &literal) {
  return literal.value;
}

int32_t Evaluator::visit(const BinaryOperator &binop) {
  int32_t left = binop.get_left().accept(*this);
  int32_t right = binop.get_right().accept(*this);
  switch (binop.op) {
    case o_plus:   return left + right;
    case o_minus:  return left - right;
    case o_times:  return left * right;
    case o_divide:
      if (right == 0)
        utils::error(binop.loc, "division by zero");
      return left / right;
    case o_eq:     return left == right ? 1 : 0;
    case o_neq:    return left != right ? 1 : 0;
    case o_lt:     return left <  right ? 1 : 0;
    case o_le:     return left <= right ? 1 : 0;
    case o_gt:     return left >  right ? 1 : 0;
    case o_ge:     return left >= right ? 1 : 0;
  }
  utils::error(binop.loc, "unknown operator");
}

int32_t Evaluator::visit(const Sequence &seq) {
  const auto &exprs = seq.get_exprs();
  if (exprs.empty())
    utils::error(seq.loc, "cannot evaluate empty sequence");
  int32_t result = 0;
  for (auto expr : exprs)
    result = expr->accept(*this);
  return result;
}

int32_t Evaluator::visit(const IfThenElse &ite) {
  int32_t cond = ite.get_condition().accept(*this);
  if (cond)
    return ite.get_then_part().accept(*this);
  else
    return ite.get_else_part().accept(*this);
}

int32_t Evaluator::visit(const StringLiteral &e) {
  utils::error(e.loc, "cannot evaluate string literal");
}
int32_t Evaluator::visit(const Let &e) {
  utils::error(e.loc, "cannot evaluate let expression");
}
int32_t Evaluator::visit(const Identifier &e) {
  utils::error(e.loc, "cannot evaluate identifier");
}
int32_t Evaluator::visit(const VarDecl &e) {
  utils::error(e.loc, "cannot evaluate variable declaration");
}
int32_t Evaluator::visit(const FunDecl &e) {
  utils::error(e.loc, "cannot evaluate function declaration");
}
int32_t Evaluator::visit(const FunCall &e) {
  utils::error(e.loc, "cannot evaluate function call");
}
int32_t Evaluator::visit(const WhileLoop &e) {
  utils::error(e.loc, "cannot evaluate while loop");
}
int32_t Evaluator::visit(const ForLoop &e) {
  utils::error(e.loc, "cannot evaluate for loop");
}
int32_t Evaluator::visit(const Break &e) {
  utils::error(e.loc, "cannot evaluate break");
}
int32_t Evaluator::visit(const Assign &e) {
  utils::error(e.loc, "cannot evaluate assignment");
}

} // namespace ast