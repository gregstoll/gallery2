			Documentation for the Picasa Module for Gallery2
			
0. ABOUT

This module lets you import albums from Picasa into Gallery2.
It will keep the order, the captions and the names.
								
Actually the module is pretty self-explanatory, but I will just 
say a few words on how to use it.

1. INSTALLATION

Copy the picasa folder into your /modules directory of 
Gallery2 and log in as administrator to install and activate 
it. 

2. EXPORTING FROM PICASA

To export from Picasa you can rightclick an album and click
on "Make a Webpage" (or you choose "Folder/Export as Webpage..." 
from the menu), from there you select the sizes, and the export 
folder location. In the next window you need to choose "XML Code",
and off it goes. Then you navigate to the folder you just chose
and beam that up on your Gallery Server.

3.1 IMPORTING INTO GALLERY2 (FROM ADD ITEM)

The new feature to this module is that any user can import files
from Picasa by simply clicking on "Add Item". A Picasa 2-Tab will
appear where a local ZIP file can be selected. That ZIP file has
to contain a picasa export (index.xml being in the root of the archive).
The user also can select how picture titles, summaries and descriptions
are assigned.

3.2 IMPORTING INTO GALLERY2 (FROM SITE ADMIN)

Once you have activated the module you get a new entry "Picasa 2"
in the Import section of your Administration menu. If you click on 
that it will let you select the local path to the Picasa export 
directory which should be stored locally (there will be upload 
support in the future). You also select the album you want to import
your album into.

Thereafter it should do everything by itself and report back to you.

4. TROUBLESHOOTING

If you have any queries, there is a great support forum at
http://gallery.menalto.com where we might be able to help.

If you spot any bugs or have feature requests please mail to 
gallery@opencodes.org
