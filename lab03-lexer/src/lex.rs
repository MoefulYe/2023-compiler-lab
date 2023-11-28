use crate::error::{LexError, LexResult, SourcedLexError};
use crate::span::{Meta, Span};
use crate::tokens::{ident_or_reserved, Token};
use miette::{miette, LabeledSpan, NamedSource, Severity};
use nom::branch::alt;
use nom::bytes::complete::{tag, tag_no_case, take_until};
use nom::character::complete::{alpha1, alphanumeric1, char, digit1, multispace0, one_of};
use nom::combinator::{iterator, map, opt, recognize};
use nom::multi::many0_count;
use nom::sequence::{delimited, pair, tuple};
use nom::Err;

pub fn lex(filename: &str, input: &str) {
    let span = Span::new_extra(input, Meta::new(filename));
    let mut it = iterator(span, lex_token);
    it.for_each(|token| println!("{token}"));
    match it.finish().err() {
        Some(Err::Failure(err)) => {
            if err.is_eof() {
                return;
            }
            let code = err.error.code();
            let msg = err.error.to_string();
            let labels = vec![LabeledSpan::at(err.span(), "here")];
            let report = miette!(
                severity = Severity::Error,
                code = code,
                labels = labels,
                "{}",
                msg
            )
            .with_source_code(NamedSource::new(filename, input.to_string()));
            println!("{:?}", report);
        }
        Some(Err::Error(_)) => {
            unreachable!()
        }
        Some(Err::Incomplete(_)) => {
            unreachable!()
        }
        None => {}
    }
}

fn raise_failure<'a>(err: LexError<'a>) -> impl FnMut(Span<'a>) -> LexResult<'a, Token> {
    move |input: Span| {
        LexResult::Err(nom::Err::Failure(SourcedLexError {
            error: err.clone(),
            span: input,
        }))
    }
}

fn eof(input: Span) -> LexResult {
    if input.is_empty() {
        LexResult::Err(nom::Err::Failure(SourcedLexError {
            error: LexError::Eof,
            span: input,
        }))
    } else {
        LexResult::Err(nom::Err::Error(SourcedLexError {
            error: LexError::Unknown,
            span: input,
        }))
    }
}

macro_rules! syntax {
    ($func_name: ident, $tag_string: literal, $output_token: expr) => {
        fn $func_name(input: Span) -> LexResult<Token> {
            map(tag($tag_string), |_| $output_token)(input)
        }
    };
}

pub fn lex_token(input: Span) -> LexResult<Token> {
    delimited(
        skip,
        alt((
            lex_lit,
            lex_operators,
            lex_punctuator,
            lex_ident_or_reserved,
            eof,
            raise_failure(LexError::Unknown),
        )),
        skip,
    )(input)
}

fn skip(input: Span) -> LexResult<()> {
    let multi_line_comment = tuple((
        multispace0,
        tag("/*"),
        take_until("*/"),
        tag("*/"),
        multispace0,
    ));
    let one_line_comment = tuple((
        multispace0,
        tag("//"),
        take_until("\n"),
        tag("\n"),
        multispace0,
    ));
    let commets = recognize(many0_count(tuple((
        alt((one_line_comment, multi_line_comment)),
        multispace0,
    ))));

    return map(tuple((multispace0, opt(commets))), |_| ())(input);
}

syntax!(lex_equal, "=", Token::Equal);
syntax!(lex_plus, "+", Token::Plus);
syntax!(lex_minus, "-", Token::Minus);
syntax!(lex_star, "*", Token::Star);
syntax!(lex_slash, "/", Token::Slash);
syntax!(lex_percent, "%", Token::Percent);
syntax!(lex_ampersand_ampersand, "&&", Token::AmpersandAmpersand);
syntax!(lex_pipe_pipe, "||", Token::PipePipe);
syntax!(lex_bang, "!", Token::Bang);
syntax!(lex_equal_equal, "==", Token::EqualEqual);
syntax!(lex_bang_equal, "!=", Token::BangEqual);
syntax!(lex_less, "<", Token::Less);
syntax!(lex_greater, ">", Token::Greater);
syntax!(lex_less_equal, "<=", Token::LessEqual);
syntax!(lex_greater_equal, ">=", Token::GreaterEqual);
syntax!(lex_tilde, "~", Token::Tilde);
syntax!(lex_caret, "^", Token::Caret);
syntax!(lex_ampersand, "&", Token::Ampersand);
syntax!(lex_pipe, "|", Token::Pipe);
syntax!(lex_comma, ",", Token::Comma);
syntax!(lex_semi_colon, ";", Token::SemiColon);
syntax!(lex_lparen, "(", Token::LParen);
syntax!(lex_rparen, ")", Token::RParen);
syntax!(lex_lbrack, "[", Token::LBrack);
syntax!(lex_rbrack, "]", Token::RBrack);
syntax!(lex_lbrace, "{", Token::LBrace);
syntax!(lex_rbrace, "}", Token::RBrace);
syntax!(lex_dot_dot_dot, "...", Token::DotDotDot);

fn lex_operators(input: Span) -> LexResult<Token> {
    alt((
        lex_equal_equal,
        lex_equal,
        lex_plus,
        lex_minus,
        lex_star,
        lex_slash,
        lex_percent,
        lex_ampersand_ampersand,
        lex_pipe_pipe,
        lex_bang_equal,
        lex_bang,
        lex_less_equal,
        lex_less,
        lex_greater_equal,
        lex_greater,
        lex_tilde,
        lex_caret,
        lex_ampersand,
        lex_pipe,
    ))(input)
}

fn lex_punctuator(input: Span) -> LexResult<Token> {
    alt((
        lex_comma,
        lex_semi_colon,
        lex_lparen,
        lex_rparen,
        lex_lbrack,
        lex_rbrack,
        lex_lbrace,
        lex_rbrace,
        lex_dot_dot_dot,
    ))(input)
}

fn lex_ident_or_reserved(input: Span) -> LexResult<Token> {
    let (leftover, ident) = recognize(pair(
        alt((tag("_"), alpha1)),
        many0_count(alt((tag("_"), alphanumeric1))),
    ))(input)?;
    return Ok((leftover, ident_or_reserved(ident.fragment())));
}

use lex_char::lex as lex_char_lit;
use lex_float::lex as lex_float_lit;
use lex_int::lex as lex_int_lit;
use lex_str::lex as lex_str_lit;

fn lex_lit(input: Span) -> LexResult<Token> {
    alt((lex_str_lit, lex_char_lit, lex_float_lit, lex_int_lit))(input)
}

mod lex_str {
    use nom::{
        branch::alt,
        bytes::complete::{is_not, take},
        character::complete::char,
        combinator::{cut, map, map_res, value, verify},
        multi::fold_many0,
        sequence::{delimited, preceded},
        Parser,
    };

    use crate::error::LexError;

    use super::{LexResult, Span, Token};

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum StringFragment<'a> {
        Literal(&'a str),
        EscapedChar(char),
    }

    fn lex_escaped(input: Span) -> LexResult<char> {
        preceded(
            char('\\'),
            cut(alt((
                value('\t', char('t')),
                value('\r', char('r')),
                value('\n', char('n')),
                value('"', char('"')),
                value('\\', char('\\')),
                map_res(take(1usize), |input: Span| {
                    Err(LexError::EscapeCharError(
                        input.fragment().chars().next().unwrap(),
                    ))
                }),
            ))),
        )(input)
    }

    fn lex_lit<'a>(input: Span<'a>) -> LexResult<&'a str> {
        let not_quote_slash = is_not("\"\\");
        let (leftover, parsed) =
            verify(not_quote_slash, |s: &Span| !s.fragment().is_empty()).parse(input)?;
        Ok((leftover, parsed.fragment()))
    }

    fn lex_fragment(input: Span) -> LexResult<StringFragment> {
        alt((
            map(lex_escaped, StringFragment::EscapedChar),
            map(lex_lit, StringFragment::Literal),
        ))(input)
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        let parse_inner = fold_many0(lex_fragment, String::new, |mut acc, fragment| {
            match fragment {
                StringFragment::Literal(s) => acc.push_str(s),
                StringFragment::EscapedChar(c) => acc.push(c),
            }
            acc
        });
        map(delimited(char('"'), parse_inner, char('"')), Token::StrLit)(input)
    }

    #[cfg(test)]
    mod test {
        use crate::span::Meta;
        use crate::tokens::Token;

        use super::lex;
        use super::Span;
        #[test]
        fn test_str() {
            assert_eq!(
                lex(Span::new_extra(r###""hello world""###, Meta::new("")))
                    .unwrap()
                    .1,
                Token::StrLit("hello world".to_owned())
            );
            assert_eq!(
                lex(Span::new_extra(r###""hello\nworld""###, Meta::new("")))
                    .unwrap()
                    .1,
                Token::StrLit("hello\nworld".to_owned())
            );
            assert_eq!(
                lex(Span::new_extra(r###""hello\tworld""###, Meta::new("")))
                    .unwrap()
                    .1,
                Token::StrLit("hello\tworld".to_owned())
            );
        }
    }
}

mod lex_char {
    use nom::branch::alt;
    use nom::bytes::complete::take;
    use nom::character::complete::char;
    use nom::combinator::cut;
    use nom::combinator::map;
    use nom::combinator::map_res;
    use nom::combinator::value;
    use nom::sequence::delimited;
    use nom::sequence::preceded;
    use nom::Parser;

    use crate::error::LexError;
    use crate::tokens::Token;

    use super::LexResult;
    use super::Span;
    fn lex_escaped(input: Span) -> LexResult<u8> {
        preceded(
            char('\\'),
            cut(alt((
                value(b'\t', char('t')),
                value(b'\r', char('r')),
                value(b'\n', char('n')),
                value(b'\'', char('\'')),
                value(b'\\', char('\\')),
                map_res(take(1usize), |input: Span| {
                    Err(LexError::EscapeCharError(input.chars().next().unwrap()))
                }),
            ))),
        )(input)
    }

    fn lex_normal(input: Span) -> LexResult<u8> {
        let (leftover, parsed) = nom::bytes::complete::take(1usize).parse(input)?;
        let ch = parsed.fragment().chars().next().unwrap();
        assert!(ch.is_ascii(), "char literal must be ascii");
        Ok((leftover, ch as u8))
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        map(
            delimited(char('\''), alt((lex_escaped, lex_normal)), char('\'')),
            Token::CharLit,
        )(input)
    }

    #[cfg(test)]
    mod test {
        use crate::span::Meta;
        use crate::tokens::Token;

        use super::lex;
        use super::Span;
        #[test]
        fn test_lex() {
            assert_eq!(
                lex(Span::new_extra("'a'", Meta::new(""))).unwrap().1,
                Token::CharLit(b'a')
            );
            assert_eq!(
                lex(Span::new_extra("'\\n'", Meta::new(""))).unwrap().1,
                Token::CharLit(b'\n')
            );
            assert_eq!(
                lex(Span::new_extra("'\\t'", Meta::new(""))).unwrap().1,
                Token::CharLit(b'\t')
            );
            assert_eq!(
                lex(Span::new_extra("'\\r'", Meta::new(""))).unwrap().1,
                Token::CharLit(b'\r')
            );
            assert_eq!(
                lex(Span::new_extra("'\\''", Meta::new(""))).unwrap().1,
                Token::CharLit(b'\'')
            );
            assert_eq!(
                lex(Span::new_extra("'\\\\'", Meta::new(""))).unwrap().1,
                Token::CharLit(b'\\')
            );
        }
    }
}

mod lex_float {
    use nom::Err;

    use crate::error::{LexError, SourcedLexError};

    use super::*;
    fn interger_fractional_expponent(input: Span) -> LexResult<Span> {
        let integer_part = pair(opt(one_of::<_, _, SourcedLexError>("+-")), digit1);
        let fractional_part = pair(char::<_, SourcedLexError>('.'), digit1);
        let exponent_part = pair(
            tag_no_case("e"),
            pair(opt(one_of::<_, _, SourcedLexError>("+-")), digit1),
        );
        recognize(tuple((integer_part, fractional_part, exponent_part)))(input)
    }

    fn interger_fractional(input: Span) -> LexResult<Span> {
        let integer_part = pair(opt(one_of::<_, _, SourcedLexError>("+-")), digit1);
        let fractional_part = pair(char::<_, SourcedLexError>('.'), digit1);
        recognize(tuple((integer_part, fractional_part)))(input)
    }

    fn interger_exponent(input: Span) -> LexResult<Span> {
        let integer_part = pair(opt(one_of::<_, _, SourcedLexError>("+-")), digit1);
        let exponent_part = pair(
            tag_no_case("e"),
            pair(opt(one_of::<_, _, SourcedLexError>("+-")), digit1),
        );
        recognize(tuple((integer_part, exponent_part)))(input)
    }

    fn no_interger_part(input: Span) -> LexResult<Span> {
        let fractional_part = pair(char::<_, SourcedLexError>('.'), digit1);
        let exponent_part = pair(
            tag_no_case("e"),
            pair(opt(one_of::<_, _, SourcedLexError>("+-")), digit1),
        );
        recognize(tuple((fractional_part, opt(exponent_part))))(input)
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        let (leftover, parsed) = alt((
            interger_fractional_expponent,
            interger_exponent,
            interger_fractional,
            no_interger_part,
        ))(input.clone())?;
        let parsed = parsed.fragment();
        let ret = parsed.parse::<f32>().map_err(|err| {
            Err::Failure(SourcedLexError {
                error: LexError::ParseFloatError(err, *parsed),
                span: input,
            })
        })?;
        Ok((leftover, Token::FloatLit(ret)))
    }

    #[cfg(test)]
    mod test {
        use crate::span::Meta;

        use super::lex;
        use super::Span;
        use super::Token;
        #[test]
        fn test_float() {
            assert_eq!(
                lex(Span::new_extra("1.0", Meta::new(""))).unwrap().1,
                Token::FloatLit(1.0)
            );
            assert_eq!(
                lex(Span::new_extra("1.0e1", Meta::new(""))).unwrap().1,
                Token::FloatLit(1.0e1)
            );
            assert_eq!(
                lex(Span::new_extra("1.0e-1", Meta::new(""))).unwrap().1,
                Token::FloatLit(1.0e-1)
            );
            assert_eq!(
                lex(Span::new_extra("1.0e+1", Meta::new(""))).unwrap().1,
                Token::FloatLit(1.0e+1)
            );
            assert_eq!(
                lex(Span::new_extra("1.0E1", Meta::new(""))).unwrap().1,
                Token::FloatLit(1.0E1)
            );
            assert_eq!(
                lex(Span::new_extra("1.0E-1", Meta::new(""))).unwrap().1,
                Token::FloatLit(1.0E-1)
            );
            assert_eq!(
                lex(Span::new_extra(".1E+1", Meta::new(""))).unwrap().1,
                Token::FloatLit(0.1E+1)
            );
        }
    }
}

mod lex_int {
    use crate::error::{LexError, SourcedLexError};

    use super::*;
    use nom::{character::complete::i32, sequence::preceded, Err};

    fn hex(input: Span) -> LexResult<i32> {
        let (leftover, parsed) = preceded(tag_no_case("0x"), alphanumeric1)(input.clone())?;
        let parsed = parsed.fragment();
        let ret = i32::from_str_radix(parsed, 16).map_err(|err| {
            Err::Failure(SourcedLexError {
                error: LexError::ParseIntError(err, *parsed),
                span: input,
            })
        })?;
        Ok((leftover, ret))
    }

    fn bin(input: Span) -> LexResult<i32> {
        let (leftover, parsed) = preceded(tag_no_case("0b"), alphanumeric1)(input.clone())?;
        let parsed = parsed.fragment();
        let ret = i32::from_str_radix(parsed, 2).map_err(|err| {
            Err::Failure(SourcedLexError {
                error: LexError::ParseIntError(err, *parsed),
                span: input,
            })
        })?;
        Ok((leftover, ret))
    }

    fn oct(input: Span) -> LexResult<i32> {
        let (leftover, parsed) = preceded(tag_no_case("0o"), alphanumeric1)(input.clone())?;
        let parsed = parsed.fragment();
        let ret = i32::from_str_radix(parsed, 8).map_err(|err| {
            Err::Failure(SourcedLexError {
                error: LexError::ParseIntError(err, *parsed),
                span: input,
            })
        })?;
        Ok((leftover, ret))
    }

    fn dec(input: Span) -> LexResult<i32> {
        i32(input)
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        map(alt((hex, bin, oct, dec)), Token::IntLit)(input)
    }

    #[cfg(test)]
    mod test {
        use crate::span::Meta;

        use super::super::Span;
        use super::lex;
        use super::Token;

        #[test]
        fn test_interger() {
            assert_eq!(
                lex(Span::new_extra("0x1", Meta::new(""))).unwrap().1,
                Token::IntLit(1)
            );
            assert_eq!(
                lex(Span::new_extra("0b1", Meta::new(""))).unwrap().1,
                Token::IntLit(1)
            );
            assert_eq!(
                lex(Span::new_extra("0o1", Meta::new(""))).unwrap().1,
                Token::IntLit(1)
            );
            assert_eq!(
                lex(Span::new_extra("1", Meta::new(""))).unwrap().1,
                Token::IntLit(1)
            );
        }
    }
}
