#ifndef INC_NoViableAltException_hpp__
#define INC_NoViableAltException_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.4/lib/cpp/antlr/NoViableAltException.hpp#1 $
 */

#include <config.hpp>
#include <RecognitionException.hpp>
#include <Token.hpp>
#include <AST.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API NoViableAltException : public RecognitionException {
public:
	const RefToken token;
	const RefAST node; // handles parsing and treeparsing

	NoViableAltException(RefAST t);
	NoViableAltException(RefToken t,const ANTLR_USE_NAMESPACE(std)string& fileName_);

	~NoViableAltException() throw() {}

	/**
	 * Returns a clean error message (no line number/column information)
	 */
	ANTLR_USE_NAMESPACE(std)string getMessage() const;
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_NoViableAltException_hpp__
