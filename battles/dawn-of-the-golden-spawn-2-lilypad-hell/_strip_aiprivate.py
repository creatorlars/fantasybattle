"""Strip AIPrivateData (PieceInfo/Illustrator) from each page so Illustrator
falls back to re-parsing the PDF content stream — which now contains our
edited text — instead of using its cached native editable representation.
"""
import pikepdf

SRC = 'dawn-of-the-golden-spawn-2-lilypad-hell.ai'

pdf = pikepdf.open(SRC, allow_overwriting_input=True)
for i, page in enumerate(pdf.pages):
    if '/PieceInfo' in page.keys():
        del page['/PieceInfo']
        print(f'page {i+1}: removed /PieceInfo')
    if '/LastModified' in page.keys():
        del page['/LastModified']
pdf.save(SRC)
print(f'saved: {SRC}')
