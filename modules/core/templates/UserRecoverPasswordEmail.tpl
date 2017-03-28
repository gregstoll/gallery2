{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->text text="Hello %s," arg1=$name}

{g->text text="You receive this email because a password recovery for %s was requested by %s at %s" arg1=$baseUrl arg2=$ip arg3=$date}

{g->text text="Your username is: %s" arg1=$userName}

{g->text text="To finish the password recovery process please click the following link and enter the required information:"}
{$recoverUrl}

{g->text text="If you did not request this recovery email, you may safely ignore it."}

{g->text text="Thank you!"}
