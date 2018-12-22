/******************************************************************************

Coral Tree wds/document.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/



/******************************************************************************
class	WDS_Paragraph : public WDS_GetString

******************************************************************************/
class WDS_Paragraph : public WDS_GetString
	{
	WDS_DECLARE_TYPE(WDS_Paragraph);

	private:

		void	SplitParagraph(EW_Event *event);
		void	MergeParagraph(long previous,EW_Event *event);
		void	SelectParagraph(long previous,long align,EW_Event *event);

		WDS_Paragraph *FindParagraph(long previous);

	public:
				WDS_Paragraph(void);

virtual	long	Operate(EW_Event *event);
virtual	void	Draw(EW_Redraw *redraw);
	};



/******************************************************************************
class	WDS_DocumentForm : public WDS_Form

******************************************************************************/
class WDS_DocumentForm : public WDS_Form
	{
	WDS_DECLARE_TYPE(WDS_DocumentForm);

	public:

				WDS_DocumentForm(void);

virtual	void	Draw(EW_Redraw *redraw);
	};



/******************************************************************************
class	WDS_Document : public WDS_ScrollRegion

******************************************************************************/
class WDS_Document : public WDS_ScrollRegion
	{
	WDS_DECLARE_TYPE(WDS_Document);

	private:

		WDS_DocumentForm	form;

	public:
				WDS_Document(void);

virtual	void	Draw(EW_Redraw *redraw);
	};
