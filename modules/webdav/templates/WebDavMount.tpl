{*
 * $Revision: 17261 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if empty($item)} {assign var=item value=$theme.item} {/if}
{* Switch between view / block layout *}
{assign var=isView value=$isView|default:true}

{g->callback type="webdav.LoadMountLink" itemId=$item.id}
{assign var=link value=$block.webdav.LoadMountLink.link}

{* Only show the block for albums *}
{if $block.webdav.LoadMountLink.hasPermission and ($isView or $item.canContainChildren)}

{if $isView}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="WebDAV Mount Instructions"} </h2>
</div>

<div class="gbBlock">
{/if}
  <div class="giDescription">
    <p>
    
      {capture name=mountLink}<a href="{$link.href}"{if isset($link.script)} onclick="{$link.script}"{/if} {$link.attrs}>{/capture} 
      {g->text text="%sClick here%s to mount Gallery on your desktop with a WebDAV client.  Documentation on mounting Gallery with WebDAV is in the %sGallery Codex%s."
               arg1=$smarty.capture.mountLink arg2="</a>"
               arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:user\">" arg4="</a>"}
    </p>
    <p>
      {capture name=connectUrl}{g->url arg1="controller=webdav.WebDav" arg2="itemId=`$item.id`" forceFullUrl=true forceSessionId=false useAuthToken=false}{/capture}
      {g->text text="Alternatively, you can enter the following URL in your WebDAV client:"}
       <a href="{$smarty.capture.connectUrl}">{$smarty.capture.connectUrl}</a>
    </p>
  </div>
{if $isView}
</div>
{/if}
{/if}
