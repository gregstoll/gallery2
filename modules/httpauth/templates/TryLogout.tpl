{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
  // <![CDATA[

  try {ldelim}
    {* http://msdn.microsoft.com/workshop/author/dhtml/reference/constants/clearauthenticationcache.asp *}
    document.execCommand("ClearAuthenticationCache");
  {rdelim} catch (exception) {ldelim}
  {rdelim}

  window.location = "{$TryLogout.scriptUrl}";

  // ]]>
</script>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="If you're not automatically redirected, %sclick here to finish logging out%s." arg1="<a href=\"`$TryLogout.hrefUrl`\">" arg2="</a>"}
  </p>
</div>
