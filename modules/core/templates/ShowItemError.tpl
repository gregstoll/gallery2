{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Configuration Error: Missing Theme"} </h2>
</div>

<div class="gbBlock">
  <h3> {g->text text="Missing Theme"} </h3>

  <p class="giDescription">
    {capture name="themeId"}
      <b>{$ShowItemError.themeId}</b>
    {/capture}
    {if empty($ShowItemError.itemId)}
      {g->text text="This page is configured to use the %s theme, but it is either inactive, not installed, or incompatible." arg1=$smarty.capture.themeId}
    {else}
      {g->text text="This album is configured to use the %s theme, but it is either inactive, not installed, or incompatible." arg1=$smarty.capture.themeId}
      {capture name="editLink"}
	<a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemEdit"
	   arg3="editPlugin=ItemEditAlbum" arg4="itemId=`$ShowItemError.itemId`" arg5="return=1"}">
      {/capture}
    {/if}

    {capture name="loginLink"}
      <a href="{g->url arg1="view=core.UserAdmin" arg2="subView=core.UserLogin" arg3="return=1"}">
    {/capture}
    {capture name="adminLink"}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"
	 arg3="mode=config" arg4="return=1"}">
    {/capture}

    {if isset($theme.isFallback) && empty($ShowItemError.itemId)}
      {if $ShowItemError.isAdmin}
	{g->text text="To fix this problem you can %sinstall or activate this theme%s or select another default theme." arg1=$smarty.capture.adminLink arg2="</a>"}
      {else}
	{g->text text="To fix this problem you can %slogin as a site administrator%s and then %sinstall or activate this theme%s or select another default theme." arg1=$smarty.capture.loginLink arg2="</a>" arg3=$smarty.capture.adminLink arg4="</a>"}
      {/if}
    {else}
      {if $ShowItemError.isAdmin}
	{g->text text="To fix this problem you can either %schoose a new theme for this album%s or %sinstall or activate this theme%s." arg1=$smarty.capture.editLink arg2="</a>" arg3=$smarty.capture.adminLink arg4="</a>"}
      {elseif $ShowItemError.canEdit}
	{g->text text="To fix this problem you can either %schoose a new theme for this album%s or %slogin as a site administrator%s and then %sinstall or activate this theme%s." arg1=$smarty.capture.editLink arg2="</a>" arg3=$smarty.capture.loginLink arg4="</a>" arg5=$smarty.capture.adminLink arg6="</a>"}
      {else}
	{g->text text="To fix this problem you can either %slogin%s and then %schoose a new theme for this album%s or %slogin as a site administrator%s and then %sinstall or activate this theme%s." arg1=$smarty.capture.loginLink arg2="</a>" arg3=$smarty.capture.editLink arg4="</a>" arg5=$smarty.capture.loginLink arg6="</a>" arg7=$smarty.capture.adminLink arg8="</a>"}
      {/if}
    {/if}
  </p>
</div>
