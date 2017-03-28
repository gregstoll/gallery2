{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<form action="{g->url}" method="post" id="publishXpForm"
 enctype="{$UserAdmin.enctype|default:"application/x-www-form-urlencoded"}">
  <div>
    <script type="text/javascript">
      // <![CDATA[
      setSubtitle("{g->text text="Set options for the photos to be added." forJavascript=true}");
      setOnBackUrl("{g->url arg1="view=publishxp.SelectAlbum" arg2="albumId=`$form.albumId`" htmlEntities=false}");
      setSubmitOnNext(true);
      setButtons(true, true, false);
      // ]]>
    </script>
    {g->hiddenFormVars}
    <input type="hidden" name="{g->formVar var="controller"}" value="publishxp.Options"/>
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
    <input type="hidden" name="{g->formVar var="form[albumId]"}" value="{$form.albumId}"/>
    <input type="hidden" name="{g->formVar var="form[action][setOptions]"}" value="1"/>
  </div>

  <div class="gbBlock">
    <h3> {g->text text="Standard Options"} </h3>
    <p class="giDescription">
      {g->text text="Now that you have chosen the destination album, the following options can be used when adding the photos to Gallery."}
    </p>

    <input type="checkbox" name="{g->formVar var="form[stripExtensions]"}" {if !empty($form.stripExtensions)}checked="checked"{/if}/>
    {g->text text="Strip file extensions?"}

    <br/>

    <input type="checkbox" name="{g->formVar var="form[setCaptions]"}" {if !empty($form.setCaptions)}checked="checked"{/if}/>
    {g->text text="Set captions on the published items?"}
  </div>

  {* Include our extra ItemAddOptions *}
  {foreach from=$Options.options item=option}
    {include file="gallery:`$option.file`" l10Domain=$option.l10Domain}
  {/foreach}
</form>
