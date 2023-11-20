use std::{
    error::Error,
    fmt::Display,
    num::{ParseFloatError, ParseIntError},
};

use crate::span::Span;
use nom::{
    error::{ContextError, FromExternalError, ParseError},
    IResult,
};
use thiserror::Error;

#[derive(Error, Debug)]
pub enum LexError<'a> {
    #[error("UnknownError: unknown error")]
    Unknown,
    #[error("UnexpectedChar: expected one of {1:?}, found `{0}`!")]
    UnexpectedChar(char, Vec<char>),
    #[error("ParseFloatError: {1}, fail to parse `{0}`!")]
    ParseFloatError(ParseFloatError, &'a str),
    #[error("ParseIntError: {1}, fail to parse `{0}`!")]
    ParseIntError(ParseIntError, &'a str),
}

#[derive(Debug)]
pub struct SourcedLexError<'a> {
    pub error: LexError<'a>,
    pub span: Span<'a>,
}

impl Display for SourcedLexError<'_> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let filename = self.span.extra.filename;
        let line = self.span.location_line();
        let column = self.span.get_column();
        write!(f, "{}:{}:{}: {}", filename, line + 1, column, self.error)
    }
}

impl Error for SourcedLexError<'_> {}

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
        let unexpected = input.fragment().chars().next().unwrap();
        SourcedLexError {
            error: LexError::UnexpectedChar(unexpected, vec![]),
            span: input,
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

pub type LexResult<'a, T> = IResult<Span<'a>, T, SourcedLexError<'a>>;

impl<'a> FromExternalError<Span<'a>, LexError<'a>> for SourcedLexError<'a> {
    fn from_external_error(input: Span<'a>, _: nom::error::ErrorKind, e: LexError<'a>) -> Self {
        SourcedLexError {
            error: e,
            span: input,
        }
    }
}
