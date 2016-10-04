# Islandora OHMS Solution Pack

An Islandora solution pack that allows for ingesting and viewing content from 
the Oral History Metadata Synchronizer (OHMS).

## Introduction

This solution pack wraps the 
[OHMS Viewer application](http://www.oralhistoryonline.org/) in an Islandora 
viewer module. The viewer application is used to produce the main content of
the web pages, whcih are then modified and themed for display inside Islandora.

## Requirements

* [Islandora](https://github.com/Islandora/islandora)
* OHMS Viewer, which is bundled with the module.

## Configuration

The admin settings for this module are available at 
`admin/islandora/islandora_viewers/ohms`. These settings are global to all OHMS
collections. 

## Metadata

This module includes a Drush command to generate Dublin Core and MODS metadata
files. It will extract the metadata from the interview XML file and save it to
the file system in files ending in .DC.xml and .MODS.xml respectively. These
files can be edited as needed before batch preprocessing and ingestion.

## Additional Metadata

Four our purposes, we required additional metadata not available in OHMS. 
Interviews may include abstracts, interviewee bios, and a list of names 
referenced. That data is managed outside of the OHMS application. If present, it
will be incorporated into the MODS.xml file when it is generated.

## Preparing Your Data

Each history should be exported from the OHMS application as an XML file. 
Objects managed by this module cannot be loaded using Islandora Batch, but a 
custom Drush command can be used instead.

When loading Interview12345.xml, the command will also look for the following 
files:

* Interview12345.ABSTRACT.txt - This is a plain text version of the interview 
abstract. Optional.

* Interview12345.BIO.txt - This is a plain text bio of the interviewee. 
Optional.

* Interview12345.DC.xml - Dublin Core metadata. This can be generated from the
OHMS XML (see below).

* Interview12345.MODS.xml - MODS metadata. This can be generated from the OHMS
XML (see below).

* Interview12345.NAMES.csv - A CSV file with a of names referenced. Optional.

* Interview12345.TN.jpg - The thumbnail image. Optional.

* Interview12345.TRANSCRIPT.pdf - A PDF transcript. Optional.

## Processing Your Data

The module includes several Drush commands to prepare, processes, and ingest
your data to Islandora. The commands expect a single argument, which is a path
on the file system, and options `--force` to overwrite existing files and 
`--recursive` to process subdirectories.

### islandora_ohms_fetch_media (iofm)

Downloads the media file from the URL in the media_url element in the OHMS XML,
if present. The media file will be stored in the same directory as the interview 
and with the same base name as the interview XML file. 

During batch processing, the media file will be ingested, if present. However, 
it will not be used the HTML produced by the viewer, unless the XML is updated 
to point to it in both the media_url element and in the mediafile markup.

Example:

`drush iofm --force --recursive /path/to/directory`

### islandora_ohms_generate_metadata (iogm)

Generates the MODS and DC files from the interview XML file. If the abstract,
bio, or names files described above exist, they will be incorporated into the 
MODS XML file. Each metadata file will saved with the same basename as the 
interview XML file, with the extension .MODS.xml or .DC.xml. 

The command accepts additional options: 

* `--mods-stylesheet=/path/to/xslt` to use an alternate XSLT to generate the 
MODS metadta

* `--dc-stylesheet=/path/to/xslt` to use an alternate XSLT to generate the DC
metadata

* `--toc-separator=string` to use string to separate items in the generated
table of contents.

Example:

`drush iogm  --force --recursive /path/to/directory`

The metadata files can be edited after they've been generated if necessary.

### islandora_ohms_batch_preprocess (iobp)

Preprocesses the interview XML files and prepares them for ingestion, along with
the metadata present.

The command accepts additional arguments:

* `--namespace=string` sets the generated object's namespace to string. Required.

* `--parent=string` sets the generated object's parent collection. Required.

Example:

`drush --user=admin iobp --namespace=ohms --parent=ohms:collection /path/to/directory`

`drush --user=admin ibi`

## Theming

If the object includes a TRANSCRIPT datastream, then a Transcript tab
will be shown on the Drupal display page. If the metadta includes the
ABSTRACT, BIO, or NAMES files as processed by the Drush command 
`islandora_ohms_generate_metadata` then a Metadata tab will be shown.

The transcript will be displayed using the PDFJS library through the 
islandora_pdfjs solution pack. Metadata and the viewer will both be themed
with files in this module's `theme/` directory.

## To Do

* More testing
* Generalize the configuration a bit to support multiple collections

## Maintainer

* [Michael Joyce](https://github.com/ubermichael)

## Development and Feedback

Pull requests and issues are welcome, as are use cases and suggestions.

## License

Copyright (C) 2016 Michael Joyce

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

