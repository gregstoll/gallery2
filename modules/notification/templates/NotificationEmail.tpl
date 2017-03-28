{*
 * $Revision: 17238 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{$body}

{capture name=footer}
{g->text text="If you no longer wish to subscribe to this notification please update your account settings at:"}
{/capture}
{$smarty.capture.footer|wordwrap:70}
{$unsubscribeUrl}
