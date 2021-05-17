#include <tao/pegtl.hpp>
namespace pegtl = TAO_PEGTL_NAMESPACE;

namespace ava {
namespace textx {
// textx grammar
struct import_stm : pegtl::if_must<TAO_PEGTL_STRING("import"), grammer_to_import> {};
struct reference_stm : pegtl::if_must<TAO_PEGTL_STRING("reference"), language_name, pegtl::opt<language_alias>> {};
struct language_alias : pegtl::if_must<TAO_PEGTL_STRING("as"), ident> {};

struct language_name : pegtl::plus<pegtl::alnum> {};
struct gramar_to_import : pegtl::plus<pegtl::alnum, pegtl::one<'.'>> {};
struct import_or_reference_stm : pegtl::sor<import_stm, reference_stm> {};
struct textx_model : pegtl::until<pegtl::eof, pegtl::star<import_or_reference_stm>, pegtl::star<textx_rule>> {};

// Rules
struct textx_rule : pegtl::seq<rule_name, pegtl::opt<rule_params>, pegtl::one<':'>, textx_rule_body, pegtl::one<';'>> {
};
struct rule_param : pegtl::seq<param_name, pegtl::opt<pegtl::one<'='>, string_value>> {};
struct rule_params : pegtl::seq<pegtl::one<'['>, rule_param, pegtl::star<pegtl::one<'|'>, sequence>> {};
using rule_param = ident;
using textx_rule_body = choice;

struct choice : pegtl::seq<sequence, pegtl::star<pegtl::one<'|'>, sequence>> {};
struct sequence : pegtl::plus<repeatable_expr> {};
struct repeatable_expr : pegtl::seq<expression, pegtl::opt<repeat_operator>, pegtl::opt<pegtl::one<'-'>>> {};
struct expression
    : pegtl::sor<assignment, pegtl::opt<syntatic_predicate>, pegtl::sor<simple_match, rule_ref, bracketed_choice>> {};
struct bracketed_choice : pegtl::seq<pegtl::one<'('>, choice, pegtl::one<')'>> {};
struct repeat_operator : pegtl::seq<pegtl::one<'*', '?', '+', '#'>, pegtl::opt<repeat_modifiers>> {};
struct repeat_modifiers
    : pegtl::seq<pegtl::one<'['>, pegtl::plus<pegtl::sor<simple_match, TAO_PEGTL_STRING("eolterm")>>, pegtl::one<']'>> {
};
struct syntatic_predicate : pegtl::one<'!', '&'> {};
struct simple_match : pegtl::sor<str_match, re_match> {};

// Assignment
struct assignment : pegtl::seq<attribute, assignment_op, assignment_rhs> {};
using attribute = ident;
struct assignment_op
    : pegtl::sor<pegtl::one<'='>, TAO_PEGTL_STRING("*="), TAO_PEGTL_STRING("+="), TAO_PEGTL_STRING("?=")> {};
struct assignment_rhs : pegtl::seq<pegtl::sor<simple_match, reference>, pegtl::opt<repeat_modifiers>> {};

// Reference
struct reference : pegtl::sor<rule_ref, obj_ref> {};
using rule_ref = ident;
struct obj_ref : pegtl::seq<pegtl::one<'['>, class_name,
                            pegtl::opt<pegtl::one<'|'>, obj_ref_rule, pegtl::opt<pegtl::one<'|'>, rrel_expression>>,
                            pegtl::one<']'>> {};
using rule_name = ident;
using obj_ref_rule = ident;
using class_name = qualified_ident;
using str_match = string_value;
struct ident : pegtl::plus<pegtl::alnum> {};

}  // namespace textx
}  // namespace ava
