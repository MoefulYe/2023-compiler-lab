use std::{
    error::Error,
    fmt::Display,
    num::{ParseFloatError, ParseIntError},
};

use crate::{span::Span, tokens::Token};
use nom::{
    error::{ContextError, FromExternalError, ParseError},
    IResult,
};
use thiserror::Error;

#[derive(Error, Debug, Clone)]
pub enum LexError<'a> {
    #[error("eof")]
    Eof,
    #[error("unknown error")]
    Unknown,
    #[error("expected one of {1:?}, found `{0}`!")]
    UnexpectedChar(char, Vec<char>),
    #[error("parse float literal {1} error, `{0}`!")]
    ParseFloatError(ParseFloatError, &'a str),
    #[error("parse int literal {1} error, `{0}`!")]
    ParseIntError(ParseIntError, &'a str),
    #[error("EscapeCharError: escape char `{0}` is not supported!")]
    EscapeCharError(char),
}

impl<'a> LexError<'a> {
    pub fn code(&self) -> &'static str {
        match self {
            LexError::Eof => "eof",
            LexError::Unknown => "unknown",
            LexError::UnexpectedChar(_, _) => "unexpected_char",
            LexError::ParseFloatError(_, _) => "parse_float_error",
            LexError::ParseIntError(_, _) => "parse_int_error",
            LexError::EscapeCharError(_) => "escape_char_error",
        }
    }
}

#[derive(Debug)]
pub struct SourcedLexError<'a> {
    pub error: LexError<'a>,
    pub span: Span<'a>,
}

impl<'a> SourcedLexError<'a> {
    pub fn is_eof(&self) -> bool {
        if let LexError::Eof = self.error {
            true
        } else {
            false
        }
    }

    pub fn span(&self) -> miette::SourceSpan {
        self.span.location_offset().into()
    }
}

impl<'a> ParseError<Span<'a>> for SourcedLexError<'a> {
    fn from_error_kind(input: Span<'a>, _: nom::error::ErrorKind) -> Self {
        SourcedLexError {
            error: LexError::Unknown,
            span: input,
        }
    }

    fn append(input: Span<'a>, _: nom::error::ErrorKind, _: Self) -> Self {
        SourcedLexError {
            error: LexError::Unknown,
            span: input,
        }
    }

    fn from_char(input: Span<'a>, _: char) -> Self {
        if let Some(ch) = input.fragment().chars().next() {
            SourcedLexError {
                error: LexError::UnexpectedChar(ch, vec![]),
                span: input,
            }
        } else {
            SourcedLexError {
                error: LexError::Eof,
                span: input,
            }
        }
    }

    fn or(mut self, mut other: Self) -> Self {
        match (&mut self.error, &mut other.error) {
            (LexError::UnexpectedChar(_, expected_a), LexError::UnexpectedChar(_, expected_b)) => {
                expected_a.append(expected_b);
                self
            }
            (LexError::UnexpectedChar(_, _), _) => self,
            _ => other,
        }
    }
}

impl ContextError<Span<'_>> for SourcedLexError<'_> {
    fn add_context(_input: Span<'_>, _ctx: &'static str, other: Self) -> Self {
        other
    }
}

pub type LexResult<'a, T = Token> = IResult<Span<'a>, T, SourcedLexError<'a>>;

impl<'a> FromExternalError<Span<'a>, LexError<'a>> for SourcedLexError<'a> {
    fn from_external_error(input: Span<'a>, _: nom::error::ErrorKind, e: LexError<'a>) -> Self {
        SourcedLexError {
            error: e,
            span: input,
        }
    }
}
