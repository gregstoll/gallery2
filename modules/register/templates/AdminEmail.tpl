{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->text text="New user registration:"}

{g->text text="    Username: %s" arg1=$username}
{g->text text="   Full name: %s" arg1=$name}
{g->text text="       Email: %s" arg1=$email}

{g->text text="Activate or delete this user here"}
{g->url arg1="view=core.SiteAdmin" arg2="subView=register.AdminSelfRegistration"
	forceFullUrl=true htmlEntities=false forceSessionId=false}
