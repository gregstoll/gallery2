{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3> {g->text text="Configuration"} </h3>

  <p class="giDescription">
    {g->text text="For ISAPI Rewrite to work Gallery needs write access to the httpd.ini file."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Httpd.ini path:"}
    </td><td>
      <input type="text" size="60" name="{g->formVar var="form[httpdini]"}" value="{$form.httpdini}"/>
      {if isset($form.error.invalidDir)}
	<div class="giError">
	  {g->text text="Invalid directory."}
	</div>
      {/if}
      {if isset($form.error.cantWrite)}
	<div class="giError">
	  {g->text text="Cant write to the httpd.ini file in that directory."}
	</div>
      {/if}
    </td>
  {if isset($AdminParser.isEmbedded)}
  </tr></table>

  <h3> {g->text text="Embedded Setup"} </h3>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Public path:"}
    </td><td>
      {$AdminParser.host}<input type="text" size="40" name="{g->formVar var="form[embeddedLocation]"}" value="{$form.embeddedLocation}"/>
      {if isset($form.error.invalidPath)}
	<div class="giError">
	  {g->text text="Invalid path."}
	</div>
      {/if}
    </td>
  {/if}
  </tr></table>
</div>
