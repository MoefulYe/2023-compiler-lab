#[derive(Debug, Clone)]
pub struct Meta<'a> {
    pub filename: &'a str,
}

impl<'a> Meta<'a> {
    pub fn new(filename: &'a str) -> Self {
        Self { filename }
    }
}

pub type Span<'a> = nom_locate::LocatedSpan<&'a str, Meta<'a>>;
