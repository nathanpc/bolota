#include <iostream>

#include <Document.h>
#include <IconField.h>

using namespace std;
using namespace Bolota;

void DumpTopic(Field *fld) {
	while (fld != NULL) {
		for (uint8_t i = 0; i < fld->Depth(); ++i)
			cout << "\t";
		cout << "- " << fld->Text() << endl;

		if (fld->HasChild())
			DumpTopic(fld->Child());
		fld = fld->Next();
	}
}

int main(int argc, char **argv) {
	cout << "Bolota Test" << endl;

	// Create example document.
	Document *doc = new Document(
		new TextField(_T("Example Document")),
		new TextField(_T("This is a test document for the Bolota application")),
		DateField::Now());

	// Populate example document.
	doc->AppendTopic(new TextField(_T("First top element")));
	TextField *tmpField = new TextField(_T("Second top element"));
	DateField *fldDate = DateField::Now();
	fldDate->SetText(_T("An example of a date time field."));
	tmpField->SetChild(new TextField(
		_T("A sub-element of the second element")))
		->SetNext(new IconField(BOLOTA_ICON_CPU,
		_T("Bolota is the best way to take notes.")))
		->SetNext(new TextField(
		_T("Yet another sub-element of the second element")))
		->SetNext(fldDate);
	doc->AppendTopic(tmpField);
	tmpField = static_cast<TextField*>(
		tmpField->SetNext(new TextField(_T("Element 1")))
		->SetNext(new TextField(_T("Element 2"))));
	tmpField->SetNext(new TextField(_T("Third top element")))
		->SetChild(new TextField(_T("A sub-element of the third element")))
		->SetChild(new TextField(_T("A sub-sub-element of the third element")));
	doc->AppendTopic(new BlankField());
	doc->AppendTopic(new TextField(_T("Fourth top element")));

	// Dump everything we have.
	DumpTopic(doc->FirstTopic());

	return 0;
}
