import pytest
from pathlib import Path


def _strip_rawstring(s: str) -> str:
    return s.rstrip().replace('R"example(', '').replace(')example"', '')


def _load_inc(file: Path) -> str:
    with open(file, 'r') as fh:
        lines = [_strip_rawstring(l) for l in fh.readlines()]
        return '\n'.join(lines)


def test_docstr_includes():
    folder = Path(__file__).parent.parent / 'src' / 'bindings' / 'docstr-snippets'
    for file in folder.glob('*.inc'):
        example = _load_inc(file)
        try:
            exec(example)
        except Exception as e:
            print(
                f'Exception while executing docstr from {file}.'
                f'\nException: {e}\nCode:\n######\n{example}\n######')
            raise e
