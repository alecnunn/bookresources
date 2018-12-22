if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FK_String_StringType]') and OBJECTPROPERTY(id, N'IsForeignKey') = 1)
ALTER TABLE [dbo].[String] DROP CONSTRAINT FK_String_StringType
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[AddStringEntry]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[AddStringEntry]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[StringContents]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[StringContents]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[AnimationString]') and OBJECTPROPERTY(id, N'IsView') = 1)
drop view [dbo].[AnimationString]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[String]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[String]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[StringType]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[StringType]
GO

CREATE TABLE [dbo].[String] (
	[StringId] [int] IDENTITY (1, 1) NOT NULL ,
	[StringValue] [varchar] (255) NOT NULL ,
	[StringTypeId] [smallint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[StringType] (
	[StringTypeId] [smallint] IDENTITY (1, 1) NOT NULL ,
	[StringTypeDesc] [varchar] (50) NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[String] WITH NOCHECK ADD 
	CONSTRAINT [PK_String] PRIMARY KEY  CLUSTERED 
	(
		[StringId]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[StringType] WITH NOCHECK ADD 
	CONSTRAINT [PK_StringType] PRIMARY KEY  CLUSTERED 
	(
		[StringTypeId]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[String] WITH NOCHECK ADD 
	CONSTRAINT [IX_String] UNIQUE  NONCLUSTERED 
	(
		[StringValue]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[String] ADD 
	CONSTRAINT [FK_String_StringType] FOREIGN KEY 
	(
		[StringTypeId]
	) REFERENCES [dbo].[StringType] (
		[StringTypeId]
	)
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


-- An example view that would be used to create what appears to be a physical table representing
-- all the animation strings within the String table.  This would be useful when doing data entry to
-- present the list of value animations to choose from.
CREATE VIEW dbo.AnimationString
AS
  SELECT     StringId, StringValue
  FROM         dbo.String
  WHERE     (StringTypeId = 1)



GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

-- Adds a new entry to the String Table, by string type

CREATE PROCEDURE AddStringEntry(@stringValue varchar(255), @stringType int)
AS
  SET NOCOUNT ON
 
  DECLARE @stringId int, @existingStringType smallint, @error int

  -- Check if the string already exists, if it does, return the existing value
  SELECT @stringId = StringId , @existingStringType = StringTypeId
  FROM String
  WHERE StringValue = @stringValue
  
  IF @@RowCount = 1
  BEGIN
    IF @existingStringType = @stringType
      BEGIN
        SELECT @stringId AS StringId
        RETURN
      END
    ELSE
      BEGIN
        RAISERROR('The specified sting exists, but as a different type than the one passed in!', 16, 1)
        SELECT 0
        RETURN
      END
  END
  
  -- The string does not exist, add it
  INSERT 
  INTO String
  VALUES (@stringValue, @stringType)
  
  -- Capture the value assigned by the database for the string id

  SET @error = @@ERROR  
  SET @stringId = @@IDENTITY
  
  -- If there was an error in the INSERT of the row, exit and return
  -- 0 as the id, meaning one was not created
  IF @error <> 0
  BEGIN
    SELECT 0 AS StringId
    RETURN
  END
  
  -- Everything went ok, so return the value of id of the newly added string
  SELECT @stringId AS StringId
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

-- Returns the entire contents of the string table for the purposes of building the lookups.  Note we
-- don't care about the type at this point, the string Ids are guaranteed to be unique, and thus have
-- no overlap
CREATE PROCEDURE StringContents
AS
  SELECT StringId, StringValue
  FROM String
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

