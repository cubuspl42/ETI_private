INSERT INTO Stale VALUES( 2012, 690, 100, 750 );
INSERT INTO Stale VALUES( 2013, 680, 100, 700 );
INSERT INTO Stale VALUES( 2014, 670, 100, 750 );
INSERT INTO Stale VALUES( 2015, 720, 100, 800 );

INSERT INTO Studenci VALUES( 155215, '95032803297', 'Jakub', 'Trzebiatowski', 1 );
INSERT INTO Studenci VALUES( 155216, '95010100001', 'Jan', 'Nowak', 1 );
INSERT INTO Studenci VALUES( 155217, '95010100001', 'Jan', 'Kowalski', 1 );
INSERT INTO Studenci VALUES( 155218, '95010100001', 'Piotr', 'Nowak', 0 );

INSERT INTO WnioskiOStypendium VALUES(
	null, 2015, 155215, '2007-05-08', '63 2490 1044 0000 4200 4249 7700'
);

INSERT INTO DaneCzlonkaRodziny VALUES(
	IDENT_CURRENT('WnioskiOStypendium'), '70010100001', 'Marek', 'Trzebiatowski', 3333
);

INSERT INTO DaneCzlonkaRodziny VALUES(
	IDENT_CURRENT('WnioskiOStypendium'), '70010100002', 'Hanna', 'Dymel-Trzebiatowska', 3333
);

INSERT INTO WyplatyMiesieczne VALUES(
	1, IDENT_CURRENT('WnioskiOStypendium'), 450
);
