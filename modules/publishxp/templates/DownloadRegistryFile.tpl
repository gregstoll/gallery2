{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
Windows Registry Editor Version 5.00

{if $DownloadRegistryFile.vistaVersion}
[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\PublishingWizard\InternetPhotoPrinting\Providers\{$DownloadRegistryFile.domain}]
{else}
[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\PublishingWizard\PublishingWizard\Providers\{$DownloadRegistryFile.domain}]
{/if}
"displayname"="{g->text text="%s at %s" arg1=$DownloadRegistryFile.title arg2=$DownloadRegistryFile.domain}"
"description"="{g->text text="Publish Your Photos and Movies to %s" arg1=$DownloadRegistryFile.title}"
"href"="{g->url arg1="view=publishxp.PublishXpLogin" forceFullUrl=1 htmlEntities=0}"
"icon"="{g->url href="favicon.ico" forceFullUrl=1 htmlEntities=0}"
